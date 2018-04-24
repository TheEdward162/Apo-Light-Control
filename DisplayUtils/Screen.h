//
// Created by klara on 21.4.18.
//

#pragma once

#include "../MZApi/Display.h"

class Display;

class Screen {
public:
    // pure virtual function
    virtual void renderScreen() = 0;

    virtual void handleKnobChange(int8_t RGBDelta[3]) = 0;
    virtual void handleKnobPress(bool RGBPressed[3]) = 0;

protected:
    Display* display;
    size_t selected = 0;

};
