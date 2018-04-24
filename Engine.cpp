#include <stdexcept>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <future>

#include "Engine.h"

#include "Misc/IOTools.h"
#include "Misc/RWMutex.h"
#include "Misc/ControlMessageQueue.h"

#include "Unit/LightUnit.h"
#include "Network/NetworkHandler.h"
#include "MZApi/DeviceInput.h"

#include "MZApi/Display.h"
#include "DisplayUtils/Colour.h"
#include "DisplayUtils/font_rom8x16.h"

// #define MZ_BOARD 1

// constants
const auto broadcastInterval = std::chrono::seconds(1);
const auto broadcastDeleteInterval = std::chrono::seconds(5);

// variables
std::vector<LightUnit> unitList;
RWMutex mutexUnitList;

ControlMessageQueue controlQueue;

std::thread networkThread;

DeviceInput deviceInput;
Display display = Display(Colour::WHITE, Colour::BLACK, Colour::LIME, font_rom8x16, unitList);

void NetworkThreadHandleBroadcastMessage(NetworkHandler::RecievedMessage* recievedMessage) {
	NetworkHandler::BroadcastMessage* bMessage = (NetworkHandler::BroadcastMessage*) recievedMessage->pMessage;

	mutexUnitList.lockRead();
	// find out if this unit is in the list
	LightUnit* contextUnit = NULL;
	// also check if there are any old units that need to be removed
	bool hasAnyToRemove = false;
	auto timeNow = std::chrono::steady_clock::now();

	// start at one, skipping thisUnit
	for (size_t i = 1; i < unitList.size(); i++) {
		if (unitList[i].ip == recievedMessage->ip) {
			contextUnit = &unitList[i];
			
			if (hasAnyToRemove)
				break;
		} else if (timeNow - unitList[i].lastNetworkBroadcastTimePoint >= broadcastDeleteInterval) {
			hasAnyToRemove = true;

			if (contextUnit != NULL)
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

	if (hasAnyToRemove) {
		mutexUnitList.lockWrite();

		unitList.erase(std::remove_if(unitList.begin() + 1, unitList.end(),
			[timeNow](const LightUnit &u) { return timeNow - u.lastNetworkBroadcastTimePoint >= broadcastDeleteInterval; }),
			unitList.end());

		mutexUnitList.unlockWrite();
	}
}
void NetworkThreadHandleControlMessage(NetworkHandler::RecievedMessage* recievedMessage) {
	NetworkHandler::ControlMessage* cMessage = (NetworkHandler::ControlMessage*) recievedMessage->pMessage;
	if (cMessage->msgType == 1) {
		printf("Changes:\n");
	} else {
		printf("Sets:\n");
	}
	printf("Ceiling: R: %d G: %d B: %d\n", cMessage->valuesCeiling[0], cMessage->valuesCeiling[1], cMessage->valuesCeiling[2]);
	printf("Wall: R: %d G: %d B: %d\n\n", cMessage->valuesWall[0], cMessage->valuesWall[1], cMessage->valuesWall[2]);
}

void NetworkThreadRun(std::future<void> death) {
	// init sockets
	NetworkHandler networkHandler = NetworkHandler();

	while (death.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
		// send broadcast
		networkHandler.broadcastUnit(unitList[0]);
		auto lastTimePoint = std::chrono::steady_clock::now();

		// send control messages
		// this should be done through some kind of a synchronized (mutexed) queue that gets filled from UI thread

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

		// draw pretty pictures
		display.handleInput(deviceInput.RGBDelta, deviceInput.RGBPressed);
		printf("%hhi %hhi %hhi", deviceInput.RGBDelta[0], deviceInput.RGBDelta[1], deviceInput.RGBDelta[2]);
		printf(", %hhi %hhi %hhi\n", deviceInput.RGBPressed[0], deviceInput.RGBPressed[1], deviceInput.RGBPressed[2]);
#else
		printf("Current status:\n");
		for (auto it = unitList.begin(); it != unitList.end(); it++) {
			std::lock_guard<std::mutex> lock(it->mutex_change);
			printf("Unix 0x%lx CEIL: 0x%x WALL: 0x%x DESC: %s\n\n", it->ip, it->rgbCeiling, it->rgbWall, it->description);
		}

		display.handleInput(NULL, NULL);
#endif
		display.redraw();

		// enqueue control message
		if (unitList.size() > 1) {
			ControlMessageQueue::ControlMessageInfo controlInfo;
			memset(&controlInfo, 0, sizeof(ControlMessageQueue::ControlMessageInfo));

			controlInfo.ip = unitList[1].ip;
			controlInfo.type = 1;
			controlInfo.valuesWall[0] = 0;
			controlInfo.valuesWall[1] = 255;
			controlInfo.valuesWall[2] = 126;

			controlQueue.enqueue(controlInfo);
		}

		mutexUnitList.unlockRead();

		// sleep
		// std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
	
	// init this unit object which will be always the first element of the unitList vector
	unitList.push_back(LightUnit(argv[1]));
	IOTools::loadImage16x16(argv[2], unitList[0].image);

	// UI thread will handle drawing and input events
	// Network thread will handle network communication, e.g. broadcasting and control messaging
	// Synchronization will work with mutexes on shared objects
	// 
	// Communication between threads (e.g. send control message, please, etc.) needs to be figured out.
	// Promises? Sounds... *puts on glasses* ...promising. Check http://en.cppreference.com/w/cpp/thread/promise

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