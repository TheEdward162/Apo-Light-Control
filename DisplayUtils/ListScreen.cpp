//
// Created by klara on 21.4.18.
//

#include "ListScreen.h"

ListScreen::ListScreen(Display *display_) {
    display = display_;
}

void ListScreen::renderScreen() {
    renderUnitList();
}

void ListScreen::renderUnitList() {
    LightUnit unit;
    int y = 2;
    int x;
    display->renderRectangle(0, y, WIDTH, y + 16, display->selectColour);
    for (int i = 0; i < display->lightUnits.size(); ++i) {
        if (i == display->lineMax) {
            break;
        }
        x = 2;
        unit = display->lightUnits[i];
        display->renderIcon(unit.image, x, y);
        x+= 18;
        display->renderText(x, y, unit.description, display->fgColour);
        y += 16;
    }
}

void ListScreen::handleKnobChange(char *RGBDelta) {}

void ListScreen::handleKnobPress(bool *RGBPressed) {}