//
// Created by klara on 21.4.18.
//

#include <sstream>

#include "Colour.h"

#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

#define R5(a) ((a & (0b11111 << 11)) >> 11)
#define G6(a) ((a & (0b111111 << 5)) >> 5)
#define B5(a) (a & 0b11111)

uint16_t Colour::rgb888to565(uint32_t rgb888) {
    uint16_t r = ((R(rgb888) >> 3) & 0b11111) << 11;
    uint16_t g = ((G(rgb888) >> 2) & 0b111111) << 5;
    uint16_t b = (B(rgb888) >> 3) & 0b11111;

    return (uint16_t) (r | g | b);
}
uint32_t Colour::rgb565to888(uint16_t rgb565) {
	// StackOverflow magic https://stackoverflow.com/questions/2442576/how-does-one-convert-16-bit-rgb565-to-24-bit-rgb888
	uint32_t r = ((R5(rgb565) * 527 + 23) >> 6);
	uint32_t g = ((G6(rgb565) * 259 + 33) >> 6);
	uint32_t b = (B5(rgb565) * 527 + 23) >> 6;

	return fromRGB(r, g, b);
}

uint8_t Colour::getR(uint32_t rgb) {
    return R(rgb);
}

uint8_t Colour::getG(uint32_t rgb) {
    return G(rgb);
}

uint8_t Colour::getB(uint32_t rgb) {
    return B(rgb);
}

uint32_t Colour::setR(uint32_t value, uint8_t newR) {
	return (value & ~0xFF0000) | (newR << 16);
}
uint32_t Colour::setG(uint32_t value, uint8_t newG) {
	return (value & ~0xFF00) | (newG << 8);
}
uint32_t Colour::setB(uint32_t value, uint8_t newB) {
	return (value & ~0xFF) | newB;
}

uint32_t Colour::changeR(uint32_t value, int16_t changeR) {
	int16_t newValue = getR(value) + changeR;
	if (newValue > 255) {
		newValue = 255;
	} else if (newValue < 0) {
		newValue = 0;
	}

	return setR(value, newValue);
}
uint32_t Colour::changeG(uint32_t value, int16_t changeG) {
	int16_t newValue = getG(value) + changeG;
	if (newValue > 255) {
		newValue = 255;
	} else if (newValue < 0) {
		newValue = 0;
	}

	return setG(value, newValue);
}
uint32_t Colour::changeB(uint32_t value, int16_t changeB) {
	int16_t newValue = getB(value) + changeB;
	if (newValue > 255) {
		newValue = 255;
	} else if (newValue < 0) {
		newValue = 0;
	}

	return setB(value, newValue);
}

uint32_t Colour::fromRGB(uint8_t r, uint8_t g, uint8_t b) {
	return (r << 16) | (g << 8) | b;
}

std::string Colour::toRGBString(uint32_t rgb) {
	std::ostringstream outStream;
	outStream << "rgb(" << (int)getR(rgb) << ", " << (int)getG(rgb) << ", " << (int)getB(rgb) << ")";
	return outStream.str();
}

std::string Colour::toHexString(uint32_t rgb) {
	std::ostringstream outStream;
	outStream << "#" << std::hex << rgb;

	return outStream.str();
}