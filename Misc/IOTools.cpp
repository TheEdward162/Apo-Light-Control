#include <sys/stat.h>

#include <iostream>
#include <fstream>

#include "IOTools.h"

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

		char pixelBuffer[3];
		int shortRed, shortGreen, shortBlue;
		for (int i = 0; i < 256; i++) {
			file.read(pixelBuffer, 3);
			if (file.gcount() != 3)
				return false;

			// using RGB 565
			shortRed = (int)((uint8_t)pixelBuffer[0]) * 5 / 8;
			shortGreen =  (int)((uint8_t)pixelBuffer[1]) * 6 / 8;
			shortBlue =  (int)((uint8_t)pixelBuffer[2]) * 5 / 8;

			buffer[i] = ((shortRed & 0b11111) << 11) | ((shortGreen & 0b111111) << 5) | (shortBlue & 0b11111);
		}

		file.close();
		return true;
	}
}