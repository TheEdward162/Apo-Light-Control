//
// Created by klara on 21.4.18.
//

#include "ListScreen.h"
#include "UnitScreen.h"

ListScreen::ListScreen(Display* display) {
    this->display = display;
}

void ListScreen::renderScreen() {
    renderUnitList();
}

void ListScreen::renderUnitList() {
    int y = 2;
    int x;
    display->renderRectangle(0, y, WIDTH, y + 16, display->selectColour);
    for (size_t i = selected; i < display->lightUnits.size(); ++i) {
        if ((i - selected) == display->lineMax) {
            break;
        }
        x = 2;
		
        LightUnit& unit = display->lightUnits[i];
        display->renderIcon(unit.image, x, y);
        x+= 18;
        display->renderText(x, y, unit.description, display->fgColour);
        y += 16;
    }
}

void ListScreen::handleKnobChange(int8_t *RGBDelta) {
    selected += RGBDelta[0];
	if (selected < 0) {
		selected = 0;
	} else if (selected > display->lightUnits.size() - 1) {
		selected = display->lightUnits.size() - 1;
	}
}

void ListScreen::handleKnobPress(bool *RGBPressed) {
    if (RGBPressed[0]) {
        display->screen = new UnitScreen(display, display->lightUnits[selected]);
    }
}