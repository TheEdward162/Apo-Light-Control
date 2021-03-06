#pragma once

#include "Screen.h"
#include "../MZApi/Display.h"

/**
	@addtogroup screens
	@{
*/

/** @brief Meow. */
class NyanScreen : public Screen {
public:
    NyanScreen(Display* display);

    void renderScreen();

    void handleKnobChange(int8_t RGBDelta[3]);
    void handleKnobPress(bool RGBPressed[3]);
};
/** @} */