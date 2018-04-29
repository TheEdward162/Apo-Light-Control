#pragma once

#include <cstdint>

#include <chrono>
#include <mutex>
#include <atomic>

class LightUnit {
public:
	LightUnit();
	LightUnit(LightUnit&& other);
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
	std::atomic_bool screenActive;
};