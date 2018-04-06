#pragma once

#include <cstdint>

#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

#define setR(a, v) (a = ((a & ~0xFF0000) | (v << 16)))
#define setG(a, v) (a = ((a & ~0xFF00) | (v << 8)))
#define setB(a, v) (a = ((a & ~0xFF) | (v << 0)))


class LightUnit {
public:
	LightUnit();
	LightUnit(const char* description);
	~LightUnit();

	uint32_t rgbCeiling = 0;
	uint32_t rgbWall = 0;

	char description[17];
	uint16_t image[256];
};