#pragma once

#include <cstdint>

#include <chrono>
#include <mutex>
#include <atomic>

/**
	@defgroup unit Unit module
	@brief MZ board and light units.
*/

/**
	@addtogroup unit
	@{
*/

/**
	@brief A class representing a light unit.
**/
class LightUnit {
public:
	/**
	 * @brief Default LightUnit constructor.
	 * */
	LightUnit();
	/**
	 * @brief LightUnit constructor with unit description.
	 * @param description Description of the light unit.
	 * */
	LightUnit(const char description[16]);
	/**
	 * @brief LightUnit constructor with ip, unit description and image as parameters.
	 * @param ip The IP address of the light unit.
	 * @param description Description of the light unit.
	 * @param image Icon for the light unit.
	 * */
	LightUnit(unsigned long ip, const char description[16], const uint16_t image[256]);
	/**
	 * @brief LightUnit constructor with complete information.
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

	/**
		@brief The RGB888 colour of the ceiling.
	*/
	uint32_t rgbCeiling = 0;
	/**
		@brief The RGB888 colour of the wall.
	*/
	uint32_t rgbWall = 0;

	/**
		@brief Unit dscription.
	*/
	char description[17];
	/**
		@brief Unit icon.
	*/
	uint16_t image[256];

	/**
		@brief Unit IP.
	*/
	unsigned long ip = 0;
	/**
		@brief Time of the last recieved broadcast.
	*/
	std::chrono::steady_clock::time_point lastNetworkBroadcastTimePoint;
	
	/**
		@brief Mutex to prevent simultaneous changing of variables.
	*/
	mutable std::mutex mutex_change;
	/**
		@brief Flag to prevent unit ereasure from unitList when it's active in UnitScreen.
	*/
	std::atomic_bool screenActive;
};
/** @} */