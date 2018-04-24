//
// Created by klara on 21.4.18.
//

#include "Colour.h"

#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

uint16_t Colour::rgb888to565(uint32_t rgb888) {
    uint8_t red = R(rgb888);
    uint8_t green = G(rgb888);
    uint8_t blue  = B(rgb888);

    uint16_t b = (blue >> 3) & 0x1f;
    uint16_t g = ((green >> 2) & 0x3f) << 5;
    uint16_t r = ((red >> 3) & 0x1f) << 11;

    return (uint16_t) (r | g | b);
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
