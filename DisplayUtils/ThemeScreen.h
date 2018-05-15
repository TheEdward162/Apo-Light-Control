#pragma once

#include "Screen.h"
#include "../MZApi/Display.h"
#include "Colour.h"

/**
	@addtogroup screens
	@{
*/

/** 
	@brief Theme screen.
	@details This screen renders list of all themes and allows changing them.
*/
class ThemeScreen : public Screen {
public:
	/**
		@brief ThemeScreen constructor
		@param[in] display Display to be bound to.
	*/
    ThemeScreen(Display* display);

    void renderScreen();

    void handleKnobChange(int8_t RGBDelta[3]);
    void handleKnobPress(bool RGBPressed[3]);

private:
	const std::vector<std::pair<std::string, std::string>> uiInputPairs = {{"input-red-turn", "Up/Down"}, {"input-red-press", "Apply"}, {"input-blue-press", "Back"}};
	const std::vector<std::array<uint16_t, 3>> themes = {{Colour::DARK_GREY, Colour::WHITE, Colour::ALMOST_GOLD}, {Colour::WHITE, Colour::BLACK, Colour::ALMOST_GOLD}};

    void renderThemeList();

	size_t scrollOffset = 0;
};
/** @} */