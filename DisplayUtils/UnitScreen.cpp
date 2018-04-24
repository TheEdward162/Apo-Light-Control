//
// Created by klara on 21.4.18.
//

#include <cstring>
#include "UnitScreen.h"
#include "Colour.h"

UnitScreen::UnitScreen(Display* display_,  LightUnit& unit_) : unit(unit_){
    display = display_;
}

void UnitScreen::renderScreen() {
    renderUnitDetail();
}


void UnitScreen::handleKnobChange(char *RGBDelta) {
    if (RGBDelta[0]) {
        void updateSelected(int delta);
    }
}

void UnitScreen::handleKnobPress(bool *RGBPressed) {
    printf("%d", RGBPressed[2]);
}

void UnitScreen::renderUnitDetail() {
    int y = 2;
    int x = 0;

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
    *sx += 16*2;
    display->renderText(*sx, *sy, "Wall RGB:", display->fgColour);
    *sx = 0;
    *sy += 16;

    // colour
    renderRGBText(sx, sy, unit.rgbWall);
}

void UnitScreen::renderCeiling(int *sx, int *sy) {
    // title
    display->renderColourSquare(*sx, *sy, Colour::rgb888to565(unit.rgbCeiling));
    *sx += 16*2;
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
        if (selected < (int) (positions.size() - 1)) {
            selected++;
        }
    }
    else {
        if (selected > 0) {
            selected--;
        }
    }

}
