#include <stdexcept>
#include <cstring>

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

void Engine::run(int argc, char** argv) {
	checkArguments(argc, argv);

	// create UI thread
	// init UI
	
	// init this unit object
	LightUnit thisUnit = LightUnit(argv[1]);
	IOTools::loadImage16x16(argv[2], thisUnit.image);

	// init sockets
	NetworkHandler networkHandler = NetworkHandler();

	// main loop
	// send broadcast
	// recieve broadcast
	// poll input events
	// send control messages

	printf("%x %x %x %x %x\n", thisUnit.image[0], thisUnit.image[1], thisUnit.image[2], thisUnit.image[3], thisUnit.image[4]);
}