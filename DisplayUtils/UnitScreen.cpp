//
// Created by klara on 21.4.18.
//

#include <cstring>

#include "../Engine.h"
#include "UnitScreen.h"
#include "ListScreen.h"
#include "Colour.h"

UnitScreen::UnitScreen(Display* display,  LightUnit& unit_) : unit(unit_){
	unit.counter_readers++;
    this->display = display;
	lastTimePacketSent = std::chrono::steady_clock::now() - maxControlPacketSendFrequency;
}

UnitScreen::~UnitScreen() {
	unit.counter_readers--;
}

void UnitScreen::renderScreen() {
    renderUnitDetail();
}

void UnitScreen::handleKnobChange(int8_t *RGBDelta) {
    if (RGBDelta[0]) {
        updateSelected(RGBDelta[0]);
    } else if (RGBDelta[1]) {
		updateSelectedValue(RGBDelta[1]);
	}
}
void UnitScreen::handleKnobPress(bool *RGBPressed) {
	if (RGBPressed[2]) {
		display->toPreviousScreen();
	}
}

void UnitScreen::renderUnitDetail() {
	std::lock_guard<std::mutex> lock(unit.mutex_change);

    int y = 2;
    int x = 0;

	// render selected
	size_t selectedY = y + positions[selected] * 16;
	display->renderRectangle(0, selectedY, WIDTH, selectedY + 16, display->selectColour);

    // render title
    display->renderText(x, y, unit.description, display->fgColour);
    x += 16 * strlen(unit.description) + 16;
    display->renderIcon(unit.image, x, y);
    y += 16 * 2;
    x = 0;

    // render walls
    renderWall(&x, &y);

    // render ceiling
    renderCeiling(&x, &y);

	// render selected value
	if (selectedValue >= 0) {
		char strBuffer[4];
		sprintf(strBuffer, "%d", selectedValue);
		size_t textWidth = strlen(strBuffer) * 16;
		display->renderText(WIDTH - textWidth - 16, selectedY, strBuffer, display->fgColour);
	}
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
    char tmp_string[7];

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
    if (selectedValue >= 0) {
		updateUnitValue(true);
		selectedValue = -1;
	}
	
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

void UnitScreen::updateSelectedValue(int delta) {
	if (selectedValue == -1) {
		uint8_t realValue;
		switch (positions[selected]) {
			case WALL_R:
				realValue = Colour::getR(unit.rgbWall);
			break;
			case WALL_G:
				realValue = Colour::getG(unit.rgbWall);
			break;
			case WALL_B:
				realValue = Colour::getB(unit.rgbWall);
			break;

			case CEILING_R:
				realValue = Colour::getR(unit.rgbCeiling);
			break;
			case CEILING_G:
				realValue = Colour::getG(unit.rgbCeiling);
			break;
			case CEILING_B:
				realValue = Colour::getB(unit.rgbCeiling);
			break;
		}
		selectedValue = realValue;
	}
	selectedValue += delta;

	if (selectedValue > 255) {
		selectedValue = 255;
	} else if (selectedValue < 0) {
		selectedValue = 0;
	}

	updateUnitValue();
}
void UnitScreen::updateUnitValue(bool force) {
	if (selectedValue >= 0) {
		if (force || unit.ip == 0 || std::chrono::steady_clock::now() - lastTimePacketSent >= maxControlPacketSendFrequency) {
			lastTimePacketSent = std::chrono::steady_clock::now();

			if (unit.ip > 0) {
				// remote unit
				ControlMessageQueue::ControlMessageInfo controlInfo;
				memset(&controlInfo, 0, sizeof(ControlMessageQueue::ControlMessageInfo));

				controlInfo.ip = unit.ip;
				controlInfo.type = 2;
				
				switch (positions[selected]) {
					case WALL_R:
						controlInfo.valuesWall[0] = selectedValue;
					break;
					case WALL_G:
						controlInfo.valuesWall[1] = selectedValue;
					break;
					case WALL_B:
						controlInfo.valuesWall[2] = selectedValue;
					break;

					case CEILING_R:
						controlInfo.valuesCeiling[0] = selectedValue;
					break;
					case CEILING_G:
						controlInfo.valuesCeiling[1] = selectedValue;
					break;
					case CEILING_B:
						controlInfo.valuesCeiling[2] = selectedValue;
					break;
				}

				Engine::controlQueue.enqueue(controlInfo);
			} else {
				// local unit
				std::lock_guard<std::mutex> lock(unit.mutex_change);
				switch (positions[selected]) {
					case WALL_R:
						unit.rgbWall = Colour::setR(unit.rgbWall, selectedValue);
					break;
					case WALL_G:
						unit.rgbWall = Colour::setG(unit.rgbWall, selectedValue);
					break;
					case WALL_B:
						unit.rgbWall = Colour::setB(unit.rgbWall, selectedValue);
					break;

					case CEILING_R:
						unit.rgbCeiling = Colour::setR(unit.rgbCeiling, selectedValue);
					break;
					case CEILING_G:
						unit.rgbCeiling = Colour::setG(unit.rgbCeiling, selectedValue);
					break;
					case CEILING_B:
						unit.rgbCeiling = Colour::setB(unit.rgbCeiling, selectedValue);
					break;
				}
			}
		}
	}
}