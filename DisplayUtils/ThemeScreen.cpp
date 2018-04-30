#include "ThemeScreen.h"

ThemeScreen::ThemeScreen(Display* display) : Screen(display) {

}

void ThemeScreen::renderScreen() {
	display->clearScreen(display->bgColour);

    renderThemeList();
	renderNagivationLine(uiInputPairs);
}

void ThemeScreen::renderThemeList() {
	int y = 0;

	size_t listStart = scrollOffset;
	size_t listEnd = std::min(themes.size(), listStart + display->lineMax);

	Screen::PLineElementVector lineElements;

	for (size_t i = listStart; i < listEnd; i++) {

		lineElements.clear();
		lineElements.emplace_back(new Screen::TextLineElement(std::to_string(i + 1) + ".", 2));
		lineElements.emplace_back(new Screen::ColorSquareLineElement(themes[i][0], 0));
		lineElements.emplace_back(new Screen::ColorSquareLineElement(themes[i][1], 0));
		lineElements.emplace_back(new Screen::ColorSquareLineElement(themes[i][2], 0));

		Screen::renderLine(y, lineElements, i == selected ? display->selectColour : -1);
		y += 16;
	}
}

void ThemeScreen::handleKnobChange(int8_t *RGBDelta) {
    size_t themeCount = themes.size();
	
	int tempSelected = (int)selected + RGBDelta[0];
	if (tempSelected < 0) {
		tempSelected += themeCount;
	}
	selected = (size_t)tempSelected;
	
	if (selected > themeCount - 1) {
		selected -= themeCount;
	}

	if (selected < scrollOffset) {
		scrollOffset = selected;
	} else if (selected > scrollOffset + display->lineMax - 1) {
		scrollOffset += selected - (scrollOffset + display->lineMax - 1);
	}
}

void ThemeScreen::handleKnobPress(bool *RGBPressed) {
	if (RGBPressed[0]) {
		display->bgColour = themes[selected][0];
		display->fgColour = themes[selected][1];
		display->selectColour = themes[selected][2];
	} else if (RGBPressed[2]) {
		display->toPreviousScreen();
	}
}