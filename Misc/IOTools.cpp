#include <sys/stat.h>

#include <iostream>
#include <fstream>

#include "IOTools.h"
#include "../DisplayUtils/Colour.h"

namespace IOTools {
	bool fileExists(const std::string& path) {
		struct stat buffer;   
 		return (stat(path.c_str(), &buffer) == 0);
	}

	bool loadImage16x16(const std::string& path, uint16_t buffer[256]) {
		std::ifstream file;
		file.open(path);

		// skip PPM magic number
		file.seekg(2);

		int width, height, maxColorValue;
		file >> width;
		file >> height;
		file >> maxColorValue;

		if (width != 16 || height != 16)
			return false;

		// skip whitespace
		file.get();

		unsigned char pixelBuffer[3];
		for (int i = 0; i < 256; i++) {
			file.read((char*) pixelBuffer, 3);
			if (file.gcount() != 3)
				return false;

			buffer[i] = Colour::rgb888to565(((uint32_t)(pixelBuffer[0]) << 16) | (pixelBuffer[1] << 8) | pixelBuffer[2]);
		}

		file.close();
		return true;
	}
}