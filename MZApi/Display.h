//
// Created by klara on 14.4.18.
//

#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include "Display.h"
#include "Mapper.h"
#include "../DisplayUtils/font_types.h"
#include "../MZApi/MZRegisters.h"
#include "../Unit/LightUnit.h"
#include "../DisplayUtils/Screen.h"

#ifndef MZ_BOARD
	#include <SDL.h>
#endif

class Screen;

#define WIDTH 480
#define HEIGHT 320

class Display {
public:
	enum ScreenType {
		LIST_SCREEN,
		UNIT_SCREEN
	};

    void renderRectangle(int left, int top, int right, int bottom, uint16_t color);

    Display(uint16_t bgColour_, uint16_t fgColour_, uint16_t selectColour_, font_descriptor_t font_, std::vector<LightUnit>& lightUnits_);
    ~Display();

	std::vector<LightUnit>& lightUnits;
    
	uint16_t buffer[HEIGHT][WIDTH];
    uint16_t fgColour, bgColour, selectColour;
    size_t lineMax;

    void handleInput(int8_t rgbDelta[3], bool knobsPressed[3]);
	
	void toListScreen();
	void toUnitScreen(LightUnit& unit);
	bool toPreviousScreen();

    void setColours(uint16_t bgColour, uint16_t fgColour, uint16_t selectColour);
    void setFont(font_descriptor_t font);

    void testDisplay();

    void renderColourSquare(int topX, int topY, uint16_t colour);
    void renderText(int topX, int topY, std::string text, uint16_t colour);
    void renderIcon(uint16_t *buffer, int topX, int topY);

    void redraw();

private:
    font_descriptor_t font;
#ifndef MZ_BOARD
	SDL_Window* sdl_win = NULL;
	int8_t sdl_knobDeltas[3] = {0, 0, 0};
	bool sdl_knobPresses[3] = {false, false, false};
#endif
	Screen* currentScreen = NULL;
	Screen* previousScreen = NULL;

    void parlcd_write_cmd(uint16_t cmd);
    void parlcd_write_data(uint16_t data);
    void renderCharacter(char character, int topX, int topY, uint16_t colour);

    Mapper mapper = Mapper(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE);

    bool getBit(uint16_t bits, int position);
    void printDisplay();
};
