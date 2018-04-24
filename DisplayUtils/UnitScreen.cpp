//
// Created by klara on 21.4.18.
//

#include <cstring>
#include "UnitScreen.h"

#include "ListScreen.h"
#include "Colour.h"

UnitScreen::UnitScreen(Display* display,  LightUnit& unit_) : unit(unit_){
    this->display = display;
}

void UnitScreen::renderScreen() {
    renderUnitDetail();
}


void UnitScreen::handleKnobChange(int8_t *RGBDelta) {
    if (RGBDelta[0]) {
        updateSelected(RGBDelta[0]);
    }
}

void UnitScreen::handleKnobPress(bool *RGBPressed) {
	if (RGBPressed[2]) {
		display->toPreviousScreen();
	}
}

void UnitScreen::renderUnitDetail() {
    int y = 2;
    int x = 0;

	// render selected
	size_t selectedY = y + positions[selected] * 16;
	display->renderRectangle(0, selectedY, WIDTH, selectedY + 16, display->selectColour);

    // render title
    display->renderText(x, y, unit.description, display->fgColour);
    x += 16*strlen(unit.description) + 16;
    display->renderIcon(unit.image, x, y);
    y += 2*16;
    x = 0;

    // render walls
    renderWall(&x, &y);

    // render ceiling
    renderCeiling(&x, &y);
}


void UnitScreen::renderWall(int *sx, int *sy) {
    // title
    display->renderColourSquare(*sx, *sy, Colour::rgb888to565(unit.rgbWall));
    *sx += 16 * 2;
    display->renderText(*sx, *sy, "Wall RGB:", display->fgColour);
    *sx = 0;
    *sy += 16;

    // colour
    renderRGBText(sx, sy, unit.rgbWall);
}

void UnitScreen::renderCeiling(int *sx, int *sy) {
    // title
    display->renderColourSquare(*sx, *sy, Colour::rgb888to565(unit.rgbCeiling));
    *sx += 16 * 2;
    display->renderText(*sx, *sy, "Ceiling RGB:", display->fgColour);
    *sx = 0;
    *sy += 16;

    // colour
    renderRGBText(sx, sy, unit.rgbCeiling);
}

void UnitScreen::renderRGBText(int *sx, int *sy, uint32_t rgb) {
    char tmp_string[6];


    // R
    sprintf(tmp_string, "R: %d", Colour::getR(rgb));
    display->renderText(*sx, *sy, tmp_string, display->fgColour);
    *sy += 16;

    // G
    sprintf(tmp_string, "G: %d", Colour::getG(rgb));
    display->renderText(*sx, *sy, tmp_string, display->fgColour);
    *sy += 16;

    // B
    sprintf(tmp_string, "B: %d", Colour::getB(rgb));
    display->renderText(*sx, *sy, tmp_string, display->fgColour);
    *sy += 16;
}

void UnitScreen::updateSelected(int delta) {
    if (delta > 0) {
        if (selected < positions.size() - 1) {
            selected++;
        }
    }
    else {
        if (selected > 0) {
            selected--;
        }
    }

}
