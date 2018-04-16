#pragma once

#include <cstdint>

#include <chrono>
#include <mutex>

#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

#define setR(a, v) (a = ((a & ~0xFF0000) | (v << 16)))
#define setG(a, v) (a = ((a & ~0xFF00) | (v << 8)))
#define setB(a, v) (a = ((a & ~0xFF) | (v << 0)))


class LightUnit {
public:
	LightUnit();
	LightUnit(LightUnit&& move);
	LightUnit(const char description[16]);
	LightUnit(unsigned long ip, const char description[16], const uint16_t image[256]);
	LightUnit(unsigned long ip, const char description[16], const uint16_t image[256], uint32_t rgbCeiling, uint32_t rgbWall);
	~LightUnit();

	LightUnit& operator=(LightUnit&& other);

	uint32_t rgbCeiling = 0;
	uint32_t rgbWall = 0;

	char description[17];
	uint16_t image[256];

	unsigned long ip = 0;
	std::chrono::steady_clock::time_point lastNetworkBroadcastTimePoint;
	// prevents both networking and UI from changing this simultaneously
	mutable std::mutex mutex_change;
};