#include <stdexcept>
#include <cstring>

#include <iostream>
#include <chrono>
#include <thread>
#include <future>

#include "Engine.h"

#include "Misc/IOTools.h"

#include "Unit/LightUnit.h"
#include "Network/NetworkHandler.h"

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

// variables
LightUnit thisUnit;
std::thread networkThread;

void NetworkThreadRun(std::future<void> death) {
	// init sockets
	NetworkHandler networkHandler = NetworkHandler();

	int i = 0;
	while (death.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
		// send broadcast
		// send control messages
		// recieve control messages and execute them

		// recieve broadcasts for some time

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void mainLoop() {
	// main loop

	while (true) {
		// poll input events
		// execute stuff based on user input
		// draw pretty pictures
		break;
	}

	// Boss is coming around. Quick, pretend to be working...
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int Engine::run(int argc, char** argv) {
	checkArguments(argc, argv);
	
	// init this unit object
	thisUnit = LightUnit(argv[1]);
	IOTools::loadImage16x16(argv[2], thisUnit.image);

	// for test.ppm this should be 0000 f800 07e0 001f ffff
	printf("%x %x %x %x %x\n", thisUnit.image[0], thisUnit.image[1], thisUnit.image[2], thisUnit.image[3], thisUnit.image[4]);

	// UI thread will handle drawing and input events
	// Network thread will handle network communication, e.g. broadcasting and control messaging
	// Synchronization will work with mutexes on shared objects
	// 
	// Communication between threads (e.g. send control message, please, etc.) needs to be figured out.
	// Promises? Sounds... *puts on glasses* ...promising. Check http://en.cppreference.com/w/cpp/thread/promise

	// stateless death event, yaay
	std::promise<void> deathPromise;
	networkThread = std::thread(NetworkThreadRun, deathPromise.get_future());

	mainLoop();

	// Death is a comin'
	deathPromise.set_value();
	networkThread.join();

	return 0;
}