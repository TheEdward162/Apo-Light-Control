#include <stdexcept>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <future>

#include "Engine.h"

#include "Misc/IOTools.h"
#include "Misc/RWMutex.h"

#include "Network/NetworkHandler.h"
#include "MZApi/DeviceInput.h"

#include "MZApi/Display.h"
#include "DisplayUtils/Colour.h"
#include "DisplayUtils/font_rom8x16.h"

// #define MZ_BOARD 1

// constants
const auto broadcastInterval = std::chrono::seconds(1);
const auto broadcastDeleteInterval = std::chrono::seconds(5);

using namespace Engine;

// variables
std::vector<LightUnit> Engine::unitList;
RWMutex mutexUnitList;

ControlMessageQueue Engine::controlQueue;

std::thread networkThread;

DeviceInput deviceInput;
Display display = Display(Colour::DARK_GREY, Colour::WHITE, Colour::ALMOST_GOLD, font_rom8x16);

void NetworkThreadHandleBroadcastMessage(NetworkHandler::RecievedMessage* recievedMessage) {
	NetworkHandler::BroadcastMessage* bMessage = (NetworkHandler::BroadcastMessage*) recievedMessage->pMessage;

	mutexUnitList.lockRead();
	// find out if this unit is in the list
	LightUnit* contextUnit = NULL;

	// start at one, skipping thisUnit
	for (size_t i = 1; i < unitList.size(); i++) {
		if (unitList[i].ip == recievedMessage->ip) {
			contextUnit = &unitList[i];
			break;
		}
	}
	
	if (contextUnit == NULL) {
		// if not, lock whole list and add new unit
		mutexUnitList.unlockRead();
		mutexUnitList.lockWrite();

		unitList.push_back(LightUnit(recievedMessage->ip, bMessage->description, bMessage->image, bMessage->rgbCeiling, bMessage->rgbWall));

		mutexUnitList.unlockWrite();
		mutexUnitList.lockRead();
	} else {
		// if yes, lock that unit and write new data
		std::lock_guard<std::mutex> lock(contextUnit->mutex_change);

		memcpy(contextUnit->description, bMessage->description, 16);
		memcpy(contextUnit->image, bMessage->image, 256 * sizeof(uint16_t));
		contextUnit->rgbCeiling = bMessage->rgbCeiling;
		contextUnit->rgbWall = bMessage->rgbWall;
		contextUnit->lastNetworkBroadcastTimePoint = recievedMessage->timePoint;
	}

	mutexUnitList.unlockRead();
}
void NetworkThreadHandleControlMessage(NetworkHandler::RecievedMessage* recievedMessage) {
	NetworkHandler::ControlMessage* cMessage = (NetworkHandler::ControlMessage*) recievedMessage->pMessage;
	
	mutexUnitList.lockRead();
	std::lock_guard<std::mutex> lock(unitList[0].mutex_change);

	if (cMessage->msgType == 1) {
		unitList[0].rgbWall = Colour::changeR(unitList[0].rgbWall, cMessage->valuesWall[0]);
		unitList[0].rgbWall = Colour::changeR(unitList[0].rgbWall, cMessage->valuesWall[1]);
		unitList[0].rgbWall = Colour::changeR(unitList[0].rgbWall, cMessage->valuesWall[2]);

		unitList[0].rgbCeiling = Colour::changeR(unitList[0].rgbCeiling, cMessage->valuesCeiling[0]);
		unitList[0].rgbCeiling = Colour::changeR(unitList[0].rgbCeiling, cMessage->valuesCeiling[1]);
		unitList[0].rgbCeiling = Colour::changeR(unitList[0].rgbCeiling, cMessage->valuesCeiling[2]);
	} else {
		if (cMessage->valuesWall[0] >= 0)
			unitList[0].rgbWall = Colour::setR(unitList[0].rgbWall, cMessage->valuesWall[0]);

		if (cMessage->valuesWall[1] >= 0)
			unitList[0].rgbWall = Colour::setG(unitList[0].rgbWall, cMessage->valuesWall[1]);

		if (cMessage->valuesWall[2] >= 0)
			unitList[0].rgbWall = Colour::setB(unitList[0].rgbWall, cMessage->valuesWall[2]);

		if (cMessage->valuesCeiling[0] >= 0)
			unitList[0].rgbCeiling = Colour::setR(unitList[0].rgbCeiling, cMessage->valuesCeiling[0]);

		if (cMessage->valuesCeiling[1] >= 0)
			unitList[0].rgbCeiling = Colour::setG(unitList[0].rgbCeiling, cMessage->valuesCeiling[1]);

		if (cMessage->valuesCeiling[2] >= 0)
			unitList[0].rgbCeiling = Colour::setB(unitList[0].rgbCeiling, cMessage->valuesCeiling[2]);
	}

	mutexUnitList.unlockRead();
}

void NetworkThreadRun(std::future<void> death) {
	// init sockets
	NetworkHandler networkHandler = NetworkHandler();

	while (death.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
		// send broadcast
		networkHandler.broadcastUnit(unitList[0]);
		auto lastTimePoint = std::chrono::steady_clock::now();

		// check for units to remove
		mutexUnitList.lockRead();
		bool hasAnyToRemove = false;
		auto timeNow = std::chrono::steady_clock::now();
		auto timePast = timeNow - broadcastDeleteInterval;
		for (size_t i = 1; i < unitList.size(); i++) {
			if (unitList[i].counter_readers == 0 && unitList[i].lastNetworkBroadcastTimePoint <= timePast) {
				hasAnyToRemove = true;
				break;
			}
		}
		mutexUnitList.unlockRead();

		if (hasAnyToRemove) {
			mutexUnitList.lockWrite();

			unitList.erase(std::remove_if(unitList.begin() + 1, unitList.end(),
				[timePast](const LightUnit &u) {
						return u.counter_readers == 0 && u.lastNetworkBroadcastTimePoint <= timePast;
					}),
				unitList.end());

			mutexUnitList.unlockWrite();
		}

		// this loop runs for the rest of the broadcastInterval to use the time between broadcasts
		// it recieves broadcasts and control messages from the network, checks command message
		// queue that gets filled from UI thread and sends them
		auto sinceLastTimePoint = std::chrono::steady_clock::now() - lastTimePoint;
		while (sinceLastTimePoint < broadcastInterval) {
			// send control messages
			if (controlQueue.hasChanged()) {
				while (controlQueue.size() > 0) {
					// fetch controlMessageInfo
					ControlMessageQueue::ControlMessageInfo controlMessageInfo = controlQueue.dequeue();

					// send control message
					NetworkHandler::ControlMessage message = networkHandler.buildControlMessage(controlMessageInfo.type, controlMessageInfo.valuesCeiling, controlMessageInfo.valuesWall);
					if (!networkHandler.sendMessage(&message, controlMessageInfo.ip)) {
						perror("Could not send control message");
					}
				}
			}

			// recieve broadcasts and control messages
			NetworkHandler::RecievedMessage recievedMessage = networkHandler.recieveMessage();
			if (recievedMessage.ip != 0) {
				std::cout << std::endl << "Recieved message from ";
				printf("%hhu %hhu %hhu %hhu", ((char*) &recievedMessage.ip)[0], ((char*) &recievedMessage.ip)[1], ((char*) &recievedMessage.ip)[2], ((char*) &recievedMessage.ip)[3]);
				std::cout << " at time " << recievedMessage.timePoint.time_since_epoch().count() << " of type " << recievedMessage.pMessage->msgType << std::endl;
				
				if (recievedMessage.pMessage->msgType == 0) {
					NetworkThreadHandleBroadcastMessage(&recievedMessage);	
				} else {
					NetworkThreadHandleControlMessage(&recievedMessage);
				}
			}

			sinceLastTimePoint = std::chrono::steady_clock::now() - lastTimePoint;
		}
	}
}

void mainLoop() {
	// main loop
	while (true) {

		mutexUnitList.lockRead();
#ifdef MZ_BOARD
		// input
		deviceInput.update();
		display.handleInput(deviceInput.RGBDelta, deviceInput.RGBPressed);
#else
		display.handleInput(NULL, NULL);
#endif
		display.redraw();

		mutexUnitList.unlockRead();
	}
}

void checkArguments(int argc, char** argv) {
	// app DESCRIPTION PATH_TO_IMAGE
	if (argc < 3) {
		throw std::runtime_error("Invalid arguments. Example: app \"Living room\" image.ppm");
	}

	if (strlen(argv[1]) > 16) {
		throw std::runtime_error("Description argument too long. Must be at most 16 bytes");
	}

	if (!IOTools::fileExists(argv[2])) {
		throw std::runtime_error("Image file does not exist");
	}
}
int Engine::run(int argc, char** argv) {
	checkArguments(argc, argv);

	unitList.reserve(5);
	
	// init this unit object which will be always the first element of the unitList vector
	unitList.emplace_back(argv[1]);
	IOTools::loadImage16x16(argv[2], unitList[0].image);

	for (int i = 0; i < 20; i++) {
		unitList.emplace_back( ("Dummy unit " + std::to_string(i + 1)).c_str() );
		IOTools::loadImage16x16("icons/" + std::to_string((i + 1) % 7) + ".ppm", unitList[i + 1].image);
		unitList[i + 1].rgbWall = (uint32_t)std::chrono::steady_clock::now().time_since_epoch().count();
	}

	// stateless death event, yaay
	std::promise<void> deathPromise;
	networkThread = std::thread(NetworkThreadRun, deathPromise.get_future());
	
	deviceInput = DeviceInput();

	mainLoop();

	// Death is a comin'
	deathPromise.set_value();
	networkThread.join();

	return 0;
}