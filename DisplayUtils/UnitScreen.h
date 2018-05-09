//
// Created by klara on 21.4.18.
//

#pragma once

#include <chrono>

#include "Screen.h"

/**
	@addtogroup screens
	@{
*/

/** 
	@brief Unit screen.
	@details This screen renders info about selected unit and allows making changes to units light settings.
*/
class UnitScreen: public Screen {
public:
	/**
		@brief UnitScreen constructor
		@param[in] display Display to be bound to.
		@param[in] unit LightUnit to be bound to.
	*/
    UnitScreen(Display* display, LightUnit& unit);
	~UnitScreen();

    void renderScreen();

    void handleKnobChange(int8_t RGBDelta[3]);
    void handleKnobPress(bool RGBPressed[3]);

private:
	enum LineIndex {
		WALL_R = 3,
		WALL_G,
		WALL_B,

		CEIL_R = 7,
		CEIL_G,
		CEIL_B,

		PRESET_LIST = 11
	};

	const std::vector<std::pair<std::string, std::string>> baseUiInputPairs = {{"input-red-turn", "Up/Down"}, {"input-green-turn", "Edit value"}, {"input-blue-press", "Back"}};
	const std::vector<std::pair<std::string, std::string>> selectedValueInputPairs = {{"input-red-turn", "Up/Down"}, {"input-green-turn", "Edit value"}, {"input-green-press", "Confirm"}, {"input-blue-press", "Back"}};
	const std::vector<std::pair<std::string, std::string>> presetListUiInputPairs = {{"input-red-turn", "Up/Down"}, {"input-red-press", "Apply wall"}, {"input-green-press", "Apply ceil"}, {"input-blue-press", "Back"}};

    const std::array<int, 7> positions = {{LineIndex::WALL_R, LineIndex::WALL_G, LineIndex::WALL_B, LineIndex::CEIL_R, LineIndex::CEIL_G, LineIndex::CEIL_B, LineIndex::PRESET_LIST}};
	const std::chrono::milliseconds maxControlPacketSendFrequency = std::chrono::milliseconds(100);
	const std::array<uint32_t, 17> presets = {{0x000000, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF,
											0x00FFFF, 0xFF0073, 0xFF8C00, 0x00FF8C, 0x0073FF, 0xE6003A,
											0x3AE600, 0x003AE6, 0x34CDFF, 0xDEADBEEF, 0xFFFFFF}};

    LightUnit& unit;
	int16_t selectedValue = -1;
	
	size_t presetSelectedValue = 0;
	size_t presetScrollOffset = 0;

	std::chrono::steady_clock::time_point lastTimePacketSent;

    void renderUnitDetail();
	size_t renderColorGroup(size_t topY, std::string name, uint32_t rgb);
	void renderPresetList(size_t topY);
	
	int16_t getCurrentRealValue();
	void updateSelected(int delta);
	int updateSelectedPresetList(int delta);
	
	void updateSelectedValue(int delta);
	void updateUnitValue(bool force=false);
	void updateUnitPreset(uint32_t preset, bool isWall);
};
/** @} */