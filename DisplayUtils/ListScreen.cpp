//
// Created by klara on 21.4.18.
//

#include "ListScreen.h"
#include "ThemeScreen.h"
#include "UnitScreen.h"
#include "NyanScreen.h"

#include "../Engine.h"
#include "Colour.h"

ListScreen::ListScreen(Display* display) : Screen(display) {

}

void ListScreen::renderScreen() {
	display->clearScreen(display->bgColour);

    renderUnitList();
	renderNagivationLine(uiInputPairs);
}

void ListScreen::renderUnitList() {
	int y = 0;
    size_t unitsCount = Engine::unitList.size();

	size_t listStart = scrollOffset;
	size_t listEnd = std::min(unitsCount, listStart + display->lineMax);

	Screen::PLineElementVector lineElements;

	auto unitListIterator = Engine::unitList.begin();
	std::advance(unitListIterator, listStart);

	for (size_t i = listStart; i < listEnd; i++) {
		LightUnit& unit = *unitListIterator;
		std::lock_guard<std::mutex> lock(unit.mutex_change);

		lineElements.clear();
		lineElements.emplace_back(new Screen::IconLineElement(unit.image, 0, 2));
		lineElements.emplace_back(new Screen::TextLineElement(unit.description, display->fgColour));
		lineElements.emplace_back(new Screen::ColorSquareLineElement(Colour::rgb888to565(unit.rgbCeiling), 0, true));
		lineElements.emplace_back(new Screen::ColorSquareLineElement(Colour::rgb888to565(unit.rgbWall), 0, true));

		Screen::renderLine(y, lineElements, i == selected ? display->selectColour : -1);
		y += 16;
		unitListIterator++;
	}
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
		scrollOffset += selected - (scrollOffset + display->lineMax - 1);
	}
}

void ListScreen::handleKnobPress(bool *RGBPressed) {
    if (RGBPressed[0] && RGBPressed[1] && RGBPressed[2]) {
		display->switchScreen(new NyanScreen(display));
	} else if (RGBPressed[0]) {
		auto lightUnitIt = Engine::unitList.begin();
		std::advance(lightUnitIt, selected);
		display->switchScreen(new UnitScreen(display, *lightUnitIt));
    } else if (RGBPressed[1]) {
		display->switchScreen(new ThemeScreen(display));
	}
}