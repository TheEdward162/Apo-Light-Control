//
// Created by klara on 21.4.18.
//

#include <cstring>

#include "../Engine.h"
#include "UnitScreen.h"
#include "ListScreen.h"
#include "Colour.h"

UnitScreen::UnitScreen(Display* display,  LightUnit& unit_) : Screen(display), unit(unit_) {
	unit.screenActive = true;
	lastTimePacketSent = std::chrono::steady_clock::now() - maxControlPacketSendFrequency;
}

UnitScreen::~UnitScreen() {
	unit.screenActive = false;
}

void UnitScreen::renderScreen() {
	display->clearScreen(display->bgColour);

    renderUnitDetail();

	if (positions[selected] == LineIndex::PRESET_LIST) {
		renderNagivationLine(presetListUiInputPairs);
	} else {
		renderNagivationLine(baseUiInputPairs);
	}
}

void UnitScreen::handleKnobChange(int8_t *RGBDelta) {
    if (RGBDelta[0]) {
		int delta = RGBDelta[0];
		if (positions[selected] == LineIndex::PRESET_LIST) {
			delta = updateSelectedPresetList(RGBDelta[0]);
		}
		if (delta != 0)
        	updateSelected(delta);
    } else if (RGBDelta[1] && positions[selected] != LineIndex::PRESET_LIST) {
		updateSelectedValue(RGBDelta[1]);
	}

	updateUnitValue();
}
void UnitScreen::handleKnobPress(bool *RGBPressed) {
	if (positions[selected] == LineIndex::PRESET_LIST) {
		if (RGBPressed[0]) {
			// preset WALL
			updateUnitPreset(presets[presetSelectedValue], true);
			return;
		} else if (RGBPressed[1]) {
			// preset CEIL
			updateUnitPreset(presets[presetSelectedValue], false);
			return;
		}
	} else {

	}
	
	if (RGBPressed[2]) {
		display->toPreviousScreen();
	}
}

void UnitScreen::renderUnitDetail() {
	std::lock_guard<std::mutex> lock(unit.mutex_change);

    int y = 0;

	// render selected
	size_t selectedY = y + positions[selected] * 16;
	if (positions[selected] != LineIndex::PRESET_LIST)
		display->renderRectangle(0, selectedY, Display::width, selectedY + 16, display->selectColour);

    // render title and icon
	Screen::renderLine(y + 8, PLineElement(new TextLineElement(unit.description, display->fgColour, 5)));
	Screen::renderLine(y, PLineElement(new IconLineElement(unit.image, 3, 0, true)));
	y += 16 * 2;
	
	y += renderColorGroup(y, "Wall RGB:", unit.rgbWall);
	y += renderColorGroup(y, "Ceiling RGB:", unit.rgbCeiling);

	// render selected value
	if (positions[selected] != LineIndex::PRESET_LIST && selectedValue >= 0) {
		Screen::renderLine(selectedY, Screen::PLineElement(new TextLineElement(std::to_string(selectedValue), display->fgColour, 0, 128 + 4, true)));
	}

	renderPresetList(y);
}

size_t UnitScreen::renderColorGroup(size_t topY, std::string name, uint32_t rgb) {
	Screen::PLineElementVector lineElements;

	lineElements.emplace_back(new ColorSquareLineElement(Colour::rgb888to565(rgb), 1, 4));
	lineElements.emplace_back(new TextLineElement(name, display->fgColour));
	Screen::renderLine(topY, lineElements);
	topY += 16;

	Screen::renderLine(topY, Screen::PLineElement(new TextLineElement("R: " + std::to_string(Colour::getR(rgb)), display->fgColour, 2)));
    topY += 16;

	Screen::renderLine(topY, Screen::PLineElement(new TextLineElement("G: " + std::to_string(Colour::getG(rgb)), display->fgColour, 2)));
    topY += 16;

	Screen::renderLine(topY, Screen::PLineElement(new TextLineElement("B: " + std::to_string(Colour::getB(rgb)), display->fgColour, 2)));
    topY += 16;

	return 16 * 4;
}

void UnitScreen::renderPresetList(size_t topY) {
	Screen::renderLine(topY, Screen::PLineElement(new TextLineElement("Presets: ", display->fgColour, 2)));
	topY += 16;

	size_t height = Display::height - topY - 16;

	if (presetSelectedValue < presetScrollOffset) {
		presetScrollOffset = presetSelectedValue;
	} else if (presetSelectedValue > presetScrollOffset + height / 16 - 1) {
		presetScrollOffset += presetSelectedValue - (presetScrollOffset + height / 16 - 1);
	}
	const bool isListSelected = positions[selected] == LineIndex::PRESET_LIST;

	size_t listStart = presetScrollOffset;
	size_t listEnd = std::min(presets.size(), listStart + height / 16);

	Screen::PLineElementVector lineElements;
	for (size_t i = listStart; i < listEnd; i++) {
		lineElements.clear();

		lineElements.emplace_back(new ColorSquareLineElement(Colour::rgb888to565(presets[i]), 2));
		lineElements.emplace_back(new TextLineElement(Colour::toRGBString(presets[i]), display->fgColour));

		Screen::renderLine(topY, lineElements, isListSelected && i == presetSelectedValue ? display->selectColour : -1);
		topY += 16;
	}
}

void UnitScreen::updateSelected(int delta) {
    if (selectedValue >= 0) {
		if (getCurrentRealValue() != selectedValue)
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

int UnitScreen::updateSelectedPresetList(int delta) {
	const int temp = (int)presetSelectedValue + delta;
	if (temp < 0) {
		return temp;
	} else if (temp >= (int)presets.size()) {
		return temp - (presets.size() - 1);
	}

	presetSelectedValue = temp;

	return 0;
}

int16_t UnitScreen::getCurrentRealValue() {
	int16_t realValue = -2;
	switch (positions[selected]) {
		case LineIndex::WALL_R:
			realValue = Colour::getR(unit.rgbWall);
		break;
		case LineIndex::WALL_G:
			realValue = Colour::getG(unit.rgbWall);
		break;
		case LineIndex::WALL_B:
			realValue = Colour::getB(unit.rgbWall);
		break;

		case LineIndex::CEIL_R:
			realValue = Colour::getR(unit.rgbCeiling);
		break;
		case LineIndex::CEIL_G:
			realValue = Colour::getG(unit.rgbCeiling);
		break;
		case LineIndex::CEIL_B:
			realValue = Colour::getB(unit.rgbCeiling);
		break;
	}

	return realValue;
}

void UnitScreen::updateSelectedValue(int delta) {
	if (selectedValue == -1) {
		selectedValue = getCurrentRealValue();
		if (selectedValue == -2) {
			selectedValue = -1;
			return;
		}
	}
	selectedValue += delta;

	if (selectedValue > 255) {
		selectedValue = 255;
	} else if (selectedValue < 0) {
		selectedValue = 0;
	}
}
void UnitScreen::updateUnitValue(bool force) {
	if (selectedValue >= 0) {
		if (force || unit.ip == 0 || (getCurrentRealValue() != selectedValue && std::chrono::steady_clock::now() - lastTimePacketSent >= maxControlPacketSendFrequency)) {
			lastTimePacketSent = std::chrono::steady_clock::now();

			if (unit.ip > 0) {
				// remote unit
				ControlMessageQueue::ControlMessageInfo controlInfo(unit.ip, 2);
				
				switch (positions[selected]) {
					case LineIndex::WALL_R:
						controlInfo.valuesWall[0] = selectedValue;
					break;
					case LineIndex::WALL_G:
						controlInfo.valuesWall[1] = selectedValue;
					break;
					case LineIndex::WALL_B:
						controlInfo.valuesWall[2] = selectedValue;
					break;

					case LineIndex::CEIL_R:
						controlInfo.valuesCeiling[0] = selectedValue;
					break;
					case LineIndex::CEIL_G:
						controlInfo.valuesCeiling[1] = selectedValue;
					break;
					case LineIndex::CEIL_B:
						controlInfo.valuesCeiling[2] = selectedValue;
					break;
				}

				Engine::controlQueue.enqueue(controlInfo);
			} else {
				// local unit
				std::lock_guard<std::mutex> lock(unit.mutex_change);
				switch (positions[selected]) {
					case LineIndex::WALL_R:
						unit.rgbWall = Colour::setR(unit.rgbWall, selectedValue);
					break;
					case LineIndex::WALL_G:
						unit.rgbWall = Colour::setG(unit.rgbWall, selectedValue);
					break;
					case LineIndex::WALL_B:
						unit.rgbWall = Colour::setB(unit.rgbWall, selectedValue);
					break;

					case LineIndex::CEIL_R:
						unit.rgbCeiling = Colour::setR(unit.rgbCeiling, selectedValue);
					break;
					case LineIndex::CEIL_G:
						unit.rgbCeiling = Colour::setG(unit.rgbCeiling, selectedValue);
					break;
					case LineIndex::CEIL_B:
						unit.rgbCeiling = Colour::setB(unit.rgbCeiling, selectedValue);
					break;
				}
			}
		}
	}
}

void UnitScreen::updateUnitPreset(uint32_t preset, bool isWall) {
	if (unit.ip == 0 || std::chrono::steady_clock::now() - lastTimePacketSent >= maxControlPacketSendFrequency) {
		if (unit.ip > 0) {
			ControlMessageQueue::ControlMessageInfo controlInfo(unit.ip, 2);

			if (isWall) {
				controlInfo.valuesWall[0] = Colour::getR(preset);
				controlInfo.valuesWall[1] = Colour::getG(preset);
				controlInfo.valuesWall[2] = Colour::getB(preset);
			} else {
				controlInfo.valuesCeiling[0] = Colour::getR(preset);
				controlInfo.valuesCeiling[1] = Colour::getG(preset);
				controlInfo.valuesCeiling[2] = Colour::getB(preset);
			}

			Engine::controlQueue.enqueue(controlInfo);
		} else {
			std::lock_guard<std::mutex> lock(unit.mutex_change);
			if (isWall) {
				unit.rgbWall = preset;
			} else {
				unit.rgbCeiling = preset;
			}
		}
	}
}