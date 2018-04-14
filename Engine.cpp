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

void NetworkThreadRun(std::future<void> death) {
	// init sockets
	NetworkHandler networkHandler = NetworkHandler();

	// send broadcast
	// recieve broadcast
	// send control messages
	int i = 0;
	while (death.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
		std::cout << "i: " << i << std::endl;
		i++;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

int Engine::run(int argc, char** argv) {
	checkArguments(argc, argv);
	
	// init this unit object
	LightUnit thisUnit = LightUnit(argv[1]);
	IOTools::loadImage16x16(argv[2], thisUnit.image);

	printf("%x %x %x %x %x\n", thisUnit.image[0], thisUnit.image[1], thisUnit.image[2], thisUnit.image[3], thisUnit.image[4]);

	// UI thread will handle drawing and input events
	// Network thread will handle network communication, e.g. broadcasting and control messaging
	// Synchronization will work with mutexes on shared objects
	// 
	// Communication between threads (e.g. send control message, please, etc.) needs to be figured out.
	// Promises? Sounds... *puts on glasses* ...promising. Check http://en.cppreference.com/w/cpp/thread/promise

	// stateless death event, yaay
	std::promise<void> deathPromise;
	std::thread networkThread(NetworkThreadRun, deathPromise.get_future());

	// main loop
	// poll input events
	// execute stuff based on user input
	// draw pretty pictures

	std::this_thread::sleep_for(std::chrono::seconds(1));
	// Death is a comin'
	deathPromise.set_value();

	networkThread.join();
	return 0;
}