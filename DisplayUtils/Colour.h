//
// Created by klara on 21.4.18.
//

#pragma once

#include <cstdint>

#include <string>

namespace Colour {
    enum {
        BLACK = 0,
        WHITE = 0xFFFF,
        RED = 0xF800,
		ORANGE = 0xFC00,
        YELLOW = 0xFF80,
        LIME = 0xB7E0,
        GREEN = 0x4FE0,
        DARK_GREEN = 0x4D24,
        BLUE = 0x051F,
        PURPLE = 0x881F,
        BROWN = 0x9260
    };

    uint8_t getR(uint32_t rgb);
    uint8_t getG(uint32_t rgb);
    uint8_t getB(uint32_t rgb);

	uint32_t setR(uint32_t value, uint8_t newValue);
	uint32_t setG(uint32_t value, uint8_t newValue);
	uint32_t setB(uint32_t value, uint8_t newValue);

	uint32_t changeR(uint32_t value, int16_t change);
	uint32_t changeG(uint32_t value, int16_t change);
	uint32_t changeB(uint32_t value, int16_t change);

	uint32_t fromRGB(uint8_t r, uint8_t g, uint8_t b);

	std::string toRGBString(uint32_t rgb);
	std::string toHexString(uint32_t rgb);

    uint16_t rgb888to565(uint32_t rgb888);
	uint32_t rgb565to888(uint16_t rgb565);
};
