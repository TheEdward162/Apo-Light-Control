//
// Created by klara on 21.4.18.
//

#pragma once

#include "Screen.h"

#include <chrono>

#define WALL_R 3
#define WALL_G 4
#define WALL_B 5
#define CEILING_R 7
#define CEILING_G 8
#define CEILING_B 9

class UnitScreen: public Screen {
public:
    UnitScreen(Display* display, LightUnit& unit);
	~UnitScreen();

    void renderScreen();

    void handleKnobChange(int8_t RGBDelta[3]);
    void handleKnobPress(bool RGBPressed[3]);

private:
    LightUnit& unit;
    std::vector<int> positions = {WALL_R, WALL_G, WALL_B, CEILING_R, CEILING_G, CEILING_B};
	int16_t selectedValue = -1;
	const std::chrono::milliseconds maxControlPacketSendFrequency = std::chrono::milliseconds(100);
	std::chrono::steady_clock::time_point lastTimePacketSent;

    void renderUnitDetail();
    void renderWall(int *sx, int *sy);
    void renderCeiling(int *sx, int *sy);
    void renderRGBText(int *sx, int *sy, uint32_t rgb);
    
	void updateSelected(int delta);
	
	void updateSelectedValue(int delta);
	void updateUnitValue(bool force=false);
};