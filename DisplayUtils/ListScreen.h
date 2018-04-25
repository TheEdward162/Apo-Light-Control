//
// Created by klara on 21.4.18.
//

#pragma once

#include "Screen.h"
#include "../MZApi/Display.h"

class ListScreen: public Screen {
public:
    ListScreen(Display* display);

    void renderScreen();

    void handleKnobChange(int8_t RGBDelta[3]);
    void handleKnobPress(bool RGBPressed[3]);

private:
    void renderUnitList();

	size_t scrollOffset = 0;
};
