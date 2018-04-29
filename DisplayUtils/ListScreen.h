//
// Created by klara on 21.4.18.
//

#pragma once

#include "Screen.h"
#include "../MZApi/Display.h"

/** @addtogroup screens
 * @{
 */

/** 
	@brief Unit screen.
	@details This screen renders list of all connected units and allows selection to pass to Unit screen.
	@see UnitScreen
*/
class ListScreen: public Screen {
public:
    ListScreen(Display* display);

    void renderScreen();

    void handleKnobChange(int8_t RGBDelta[3]);
    void handleKnobPress(bool RGBPressed[3]);

private:
	const std::vector<std::pair<std::string, std::string>> uiInputPairs = {{"input-red-turn", "Up/Down"}, {"input-red-press", "Edit"}};

    void renderUnitList();

	size_t scrollOffset = 0;
};
/** @} */