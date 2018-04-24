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
	int x = 0;
	int y = 2;
    size_t unitsCount = display->lightUnits.size();
	size_t shiftCount = 0;
	if (selected > display->lineMax) {
		shiftCount = display->lineMax - selected;
	}

	for (size_t i = shiftCount; i < unitsCount; i++) {
		x = 2;

		if (i == selected) {
			display->renderRectangle(0, y, WIDTH, y + 16, display->selectColour);
		}

		display->renderIcon(display->lightUnits[i].image, x, y);
		x += 18;

		display->renderText(x, y, display->lightUnits[i].description, display->fgColour);
        y += 16;
	}

	renderNagivationLine();
}

void ListScreen::handleKnobChange(int8_t *RGBDelta) {
    int tempSelected = (int)selected + RGBDelta[0];
	if (tempSelected < 0) {
		tempSelected = 0;
	}
	selected = (size_t)tempSelected;
	
	if (selected > display->lightUnits.size() - 1) {
		selected = display->lightUnits.size() - 1;
	}
}

void ListScreen::handleKnobPress(bool *RGBPressed) {
    if (RGBPressed[0]) {
		display->toUnitScreen(display->lightUnits[selected]);
    }
}