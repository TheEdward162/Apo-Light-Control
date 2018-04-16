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

#include "Unit/LightUnit.h"
#include "Network/NetworkHandler.h"
#include "MZApi/DeviceInput.h"

// constants
const auto broadcastInterval = std::chrono::seconds(1);
const auto broadcastDeleteInterval = std::chrono::seconds(5);

// variables
LightUnit thisUnit;

std::vector<LightUnit> unitList;
RWMutex mutexUnitList;

std::thread networkThread;

DeviceInput deviceInput;

void NetworkThreadHandleBroadcastMessage(NetworkHandler::RecievedMessage* recievedMessage) {
	NetworkHandler::BroadcastMessage* bMessage = (NetworkHandler::BroadcastMessage*) recievedMessage->pMessage;
	std::cout << "Description: ";
	for (int i = 0; i < 16; i++) {
		std::cout << bMessage->description[i];
	}
	std::cout << std::endl;

	mutexUnitList.lockRead();
	// find out if this unit is in the list
	LightUnit* contextUnit = NULL;
	// also check if there are any old units that need to be removed
	bool hasAnyToRemove = false;
	auto timeNow = std::chrono::steady_clock::now();

	for (size_t i = 0; i < unitList.size(); i++) {
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

		unitList.erase(std::remove_if(unitList.begin(), unitList.end(),
			[timeNow](const LightUnit &u) { return timeNow -  u.lastNetworkBroadcastTimePoint >= broadcastDeleteInterval; }),
			unitList.end());

		mutexUnitList.unlockWrite();
	}
}
void NetworkThreadHandleControlMessage(NetworkHandler::RecievedMessage* recievedMessage) {
	NetworkHandler::ControlMessage* cMessage = (NetworkHandler::ControlMessage*) recievedMessage->pMessage;
	if (cMessage->msgType == 1) {
		std::cout << "Changes: " << std::endl;
	} else {
		std::cout << "Sets: " << std::endl;
	}
	std::cout << "Ceiling: " << "R: " << cMessage->valuesCeiling[0] << "G: " << cMessage->valuesCeiling[1] << "B: " << cMessage->valuesCeiling[2] << std::endl;
	std::cout << "Wall: " << "R: " << cMessage->valuesWall[0] << "G: " << cMessage->valuesWall[1] << "B: " << cMessage->valuesWall[2] << std::endl;
	std::cout << std::endl;
	
	std::cout << std::endl;
}

void NetworkThreadRun(std::future<void> death) {
	long a = 0x01020304;
	printf("%hhx %hhx %hhx %hhx\n", ((char*) &a)[0], ((char*) &a)[1], ((char*) &a)[2], ((char*) &a)[3]);

	// init sockets
	NetworkHandler networkHandler = NetworkHandler();

	while (death.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
		// send broadcast
		networkHandler.broadcastUnit(thisUnit);
		auto lastTimePoint = std::chrono::steady_clock::now();

		// send control messages
		// this should be done through some kind of a synchronized (mutexed) queue that gets filled from UI thread
	
		auto sinceLastTimePoint = std::chrono::steady_clock::now() - lastTimePoint;
		while (sinceLastTimePoint < broadcastInterval) {
			// recieve broadcasts and control messages for some time
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
	int iters = 20;
	while (true) {
		// poll input events
		// execute stuff based on user input
		// draw pretty pictures

		mutexUnitList.lockRead();

		std::cout << "Current status: " << std::endl;

		std::lock_guard<std::mutex> lock(thisUnit.mutex_change);
		std::cout << "Unit 0x" << std::hex << thisUnit.ip << " CEIL: 0x"  << thisUnit.rgbCeiling << " WALL: 0x" << thisUnit.rgbWall << " DESC: " << thisUnit.description << std::endl;
		lock.~lock_guard();

		for (auto it = unitList.begin(); it != unitList.end(); it++) {
			std::lock_guard<std::mutex> lock(it->mutex_change);
			std::cout << "Unit 0x" << it->ip << " CEIL: 0x" << it->rgbCeiling << " WALL: 0x" << it->rgbWall << " DESC: " << it->description << std::endl;
		}
		std::cout << std::dec << std::endl;

		mutexUnitList.unlockRead();

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		if (--iters < 0)
			break;
	}

	// Boss is coming around. Quick, pretend to be working...
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
	
	// init this unit object
	thisUnit = LightUnit(argv[1]);
	IOTools::loadImage16x16(argv[2], thisUnit.image);

	// for test.ppm this should be 0000 f800 07e0 001f ffff
	// printf("%x %x %x %x %x\n", thisUnit.image[0], thisUnit.image[1], thisUnit.image[2], thisUnit.image[3], thisUnit.image[4]);

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
	deviceInput.testDI();

	mainLoop();

	// Death is a comin'
	deathPromise.set_value();
	networkThread.join();

	return 0;
}