//
// Created by klara on 21.4.18.
//

#include "ListScreen.h"
#include "UnitScreen.h"
#include "NyanScreen.h"

#include "../Engine.h"
#include "Colour.h"

ListScreen::ListScreen(Display* display) : Screen(display) {

}

void ListScreen::renderScreen() {
	display->clearScreen(display->bgColour);

    renderUnitList();
}

void ListScreen::renderUnitList() {
	int x = 0;
	int y = 0;
    size_t unitsCount = Engine::unitList.size();

	size_t listStart = scrollOffset;
	size_t listEnd = std::min(unitsCount, listStart + display->lineMax);

	for (size_t i = listStart; i < listEnd; i++) {
		LightUnit& unit = Engine::unitList[i];
		std::lock_guard<std::mutex> lock(unit.mutex_change);

		x = 2;

		if (i == selected) {
			display->renderRectangle(0, y, Display::width, y + 16, display->selectColour);
		}

		display->renderIcon(unit.image, x, y);
		x += 18;

		display->renderText(x, y, unit.description, display->fgColour);

		// render current colors
		display->renderColourSquare(Display::width - 32, y, Colour::rgb888to565(unit.rgbWall));
		display->renderColourSquare(Display::width - 16, y, Colour::rgb888to565(unit.rgbCeiling));

        y += 16;
	}

	renderNagivationLine();
}

void ListScreen::handleKnobChange(int8_t *RGBDelta) {
    size_t unitCount = Engine::unitList.size();
	
	int tempSelected = (int)selected + RGBDelta[0];
	if (tempSelected < 0) {
		tempSelected += unitCount;
	}
	selected = (size_t)tempSelected;
	
	if (selected > unitCount - 1) {
		selected -= unitCount;
	}

	if (selected < scrollOffset) {
		scrollOffset = selected;
	} else if (selected > scrollOffset + display->lineMax - 1) {
		scrollOffset += selected - scrollOffset - display->lineMax + 1;
	}
}

void ListScreen::handleKnobPress(bool *RGBPressed) {
    if (RGBPressed[0] && RGBPressed[1] && RGBPressed[2]) {
		display->switchScreen(new NyanScreen(display));
	} else if (RGBPressed[0]) {
		display->switchScreen(new UnitScreen(display, Engine::unitList[selected]));
    }
}