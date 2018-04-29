#pragma once

#include <cstdint>

/**
	@brief Namespace that handles LEDs.
*/
namespace LedController {
	/**
		@brief Sets the color of the first led.
		@param[in] color
	*/
	void setLED1(uint32_t color);
	/**
		@brief Sets the color of the second led.
		@param[in] color
	*/
	void setLED2(uint32_t color);
	/**
		@brief Sets the on/off state of ledline leds.
		@param[in] bits Each bit represents the state of one led.
	*/
	void setLEDLine(uint32_t bits);
}
/** namespace LedController */