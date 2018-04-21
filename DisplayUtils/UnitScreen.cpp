//
// Created by klara on 21.4.18.
//

#include "UnitScreen.h"

UnitScreen::UnitScreen(Display* display_,  LightUnit& unit_) {
    display = display_;
    unit = unit_;
}

void UnitScreen::renderScreen() {}

void UnitScreen::handleKnobChange(char *RGBDelta) {}

void UnitScreen::handleKnobPress(bool *RGBPressed) {}