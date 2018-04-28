#pragma once

#include <cstdint>

#include <chrono>
#include <mutex>
#include <atomic>

/**
 * A class representing a light unit.
 * */
class LightUnit {
public:
	/**
	 * Default LightUnit constructor.
	 * */
	LightUnit();
	/**
	 * LightUnit constructor which clones another Light unit.
	 * @param other The light unit to be cloned.
	 * */
	LightUnit(LightUnit&& other);
	/**
	 * LightUnit constructor with unit description.
	 * @param description Description of the light unit.
	 * */
	LightUnit(const char description[16]);
	/**
	 * LightUnit constructor with ip, unit description and image as parameters.
	 * @param ip The IP address of the light unit.
	 * @param description Description of the light unit.
	 * @param image Icon for the light unit.
	 * */
	LightUnit(unsigned long ip, const char description[16], const uint16_t image[256]);
	/**
	 * LightUnit constructor with complete information.
	 * @param ip The IP address of the light unit.
	 * @param description Description of the light unit.
	 * @param image Icon for the light unit.
	 * @param rgbCeiling RGB888 colour of the light unit ceiling.
	 * @param rgbWall RGB888 colour of the light unit wall.
	 * */
	LightUnit(unsigned long ip, const char description[16], const uint16_t image[256], uint32_t rgbCeiling, uint32_t rgbWall);
	/**
	 * LightUnit destructor.
	 * */
	~LightUnit();

	LightUnit& operator=(LightUnit&& other);

	uint32_t rgbCeiling = 0; /**< The RGB888 colour of the ceiling. */
	uint32_t rgbWall = 0; /**< The RGB888 colour of the wall. */

	char description[17]; /**< Unit dscription. */
	uint16_t image[256]; /**< Unit icon. */

	unsigned long ip = 0; /**< Unit IP. */
	std::chrono::steady_clock::time_point lastNetworkBroadcastTimePoint; /**< Time of the last broadcast. */
	mutable std::mutex mutex_change;
	std::atomic<std::uint32_t> counter_readers;
};