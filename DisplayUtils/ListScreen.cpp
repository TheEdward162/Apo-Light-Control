//
// Created by klara on 21.4.18.
//

#include "ListScreen.h"
#include "UnitScreen.h"

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
    for (int i = selected; i < display->lightUnits.size(); ++i) {
        if ((i - selected) == display->lineMax) {
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

void ListScreen::handleKnobChange(char *RGBDelta) {
    selected+= RGBDelta[0];
    selected = selected < 0 ? 0 : selected;
    selected = selected > display->lightUnits.size() - 1 ?  display->lightUnits.size() - 1 : selected;
}

void ListScreen::handleKnobPress(bool *RGBPressed) {
    if (RGBPressed[0]) {
        display->screen = new UnitScreen(display, display->lightUnits[selected]);
    }
}