//
// Created by klara on 21.4.18.
//

#include "Colour.h"
#include <cstdio>

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

	return (r << 16) | (g << 8) | (b);
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
