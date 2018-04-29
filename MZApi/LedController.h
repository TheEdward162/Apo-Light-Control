#pragma once

#include <cstdint>

namespace LedController {
	void setLED1(uint32_t color);
	void setLED2(uint32_t color);
	void setLEDLine(uint32_t bits);
}