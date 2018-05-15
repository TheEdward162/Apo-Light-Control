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
#include "MZApi/LedController.h"

#include "MZApi/Display.h"
#include "DisplayUtils/Colour.h"
#include "DisplayUtils/font_rom8x16.h"

// #define MZ_BOARD 1

// constants
const auto broadcastInterval = std::chrono::seconds(1);
const auto broadcastDeleteInterval = std::chrono::seconds(5);

using namespace Engine;

// variables
std::unordered_map<std::string, std::array<uint16_t, 256>> Engine::uiIcons;

std::list<LightUnit> Engine::unitList;
RWMutex mutexUnitList;

ControlMessageQueue Engine::controlQueue;
std::thread networkThread;

DeviceInput deviceInput;
Display display = Display(Colour::WHITE, Colour::BLACK, Colour::ALMOST_GOLD, font_rom8x16, &deviceInput);

void NetworkThreadHandleBroadcastMessage(NetworkHandler::RecievedMessage* recievedMessage) {
	NetworkHandler::BroadcastMessage* bMessage = (NetworkHandler::BroadcastMessage*) recievedMessage->pMessage;

	mutexUnitList.lockRead();
	// find out if this unit is in the list, List.end() returs an iterator AFTER the last element
	auto contextUnit = unitList.end();

	// start at one, skipping thisUnit
	for (auto it = ++unitList.begin(); it != unitList.end(); it++) {
		if (it->ip == recievedMessage->ip) {
			contextUnit = it;
			break;
		}
	}
	
	if (contextUnit == unitList.end()) {
		// if not, lock whole list and add new unit
		mutexUnitList.unlockRead();
		mutexUnitList.lockWrite();

		unitList.emplace_back(recievedMessage->ip, bMessage->description, bMessage->image, bMessage->rgbCeiling, bMessage->rgbWall);

		mutexUnitList.unlockWrite();
		return;
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

	LightUnit& thisUnit = *unitList.begin();
	std::lock_guard<std::mutex> lock(thisUnit.mutex_change);

	if (cMessage->msgType == 1) {
		thisUnit.rgbWall = Colour::changeR(thisUnit.rgbWall, cMessage->valuesWall[0]);
		thisUnit.rgbWall = Colour::changeR(thisUnit.rgbWall, cMessage->valuesWall[1]);
		thisUnit.rgbWall = Colour::changeR(thisUnit.rgbWall, cMessage->valuesWall[2]);

		thisUnit.rgbCeiling = Colour::changeR(thisUnit.rgbCeiling, cMessage->valuesCeiling[0]);
		thisUnit.rgbCeiling = Colour::changeR(thisUnit.rgbCeiling, cMessage->valuesCeiling[1]);
		thisUnit.rgbCeiling = Colour::changeR(thisUnit.rgbCeiling, cMessage->valuesCeiling[2]);
	} else {
		if (cMessage->valuesWall[0] >= 0)
			thisUnit.rgbWall = Colour::setR(thisUnit.rgbWall, cMessage->valuesWall[0]);

		if (cMessage->valuesWall[1] >= 0)
			thisUnit.rgbWall = Colour::setG(thisUnit.rgbWall, cMessage->valuesWall[1]);

		if (cMessage->valuesWall[2] >= 0)
			thisUnit.rgbWall = Colour::setB(thisUnit.rgbWall, cMessage->valuesWall[2]);

		if (cMessage->valuesCeiling[0] >= 0)
			thisUnit.rgbCeiling = Colour::setR(thisUnit.rgbCeiling, cMessage->valuesCeiling[0]);

		if (cMessage->valuesCeiling[1] >= 0)
			thisUnit.rgbCeiling = Colour::setG(thisUnit.rgbCeiling, cMessage->valuesCeiling[1]);

		if (cMessage->valuesCeiling[2] >= 0)
			thisUnit.rgbCeiling = Colour::setB(thisUnit.rgbCeiling, cMessage->valuesCeiling[2]);
	}

	mutexUnitList.unlockRead();
}

void NetworkThreadRun(std::future<void> death) {
	// init sockets
	NetworkHandler networkHandler = NetworkHandler();

	while (death.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
		// send broadcast
		networkHandler.broadcastUnit(*unitList.begin());
		auto lastTimePoint = std::chrono::steady_clock::now();

		// check for units to remove
		mutexUnitList.lockRead();
		bool hasAnyToRemove = false;
		auto timeNow = std::chrono::steady_clock::now();
		auto timePast = timeNow - broadcastDeleteInterval;
		for (auto it = ++unitList.begin(); it != unitList.end(); it++) {
			if (!it->screenActive && it->ip != 0 && it->lastNetworkBroadcastTimePoint <= timePast) {
				hasAnyToRemove = true;
				break;
			}
		}
		mutexUnitList.unlockRead();

		if (hasAnyToRemove) {
			mutexUnitList.lockWrite();

			unitList.remove_if([timePast](const LightUnit &u) {
						return !u.screenActive && u.ip != 0 && u.lastNetworkBroadcastTimePoint <= timePast;
					});

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
#ifdef MZ_BOARD
	LightUnit& thisUnit = *unitList.begin();
#endif

	// main loop
	while (true) {

		mutexUnitList.lockRead();
#ifdef MZ_BOARD
		// input
		deviceInput.update();

		// update leds
		LedController::setLED1(thisUnit.rgbWall);
		LedController::setLED2(thisUnit.rgbCeiling);
#endif
		display.handleInput();
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

void loadUIIcon(std::string path, std::string key) {
	if (IOTools::fileExists(path)) {
		std::array<uint16_t, 256> iconBuffer = std::array<uint16_t, 256>();
		IOTools::loadImage16x16(path, iconBuffer.data());
		uiIcons[key] = std::move(iconBuffer);
	}
}
void loadUIIcons() {
	loadUIIcon("icons/input-red-turn.ppm", "input-red-turn");
	loadUIIcon("icons/input-red-press.ppm", "input-red-press");

	loadUIIcon("icons/input-green-turn.ppm", "input-green-turn");
	loadUIIcon("icons/input-green-press.ppm", "input-green-press");

	loadUIIcon("icons/input-blue-turn.ppm", "input-blue-turn");
	loadUIIcon("icons/input-blue-press.ppm", "input-blue-press");
}

int Engine::run(int argc, char** argv) {
	checkArguments(argc, argv);
	loadUIIcons();
	
	// init this unit object which will be always the first element of the unitList vector
	unitList.emplace_back(argv[1]);
	IOTools::loadImage16x16(argv[2], unitList.back().image);

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