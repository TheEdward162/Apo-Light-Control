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
#include <SDL2/SDL.h>
#endif

class Screen;

class Display {
public:
	static const size_t width = 480;
	static const size_t height = 320;

    Display(uint16_t bgColour, uint16_t fgColour, uint16_t selectColour, font_descriptor_t font);
    ~Display();
    
    uint16_t fgColour, bgColour, selectColour;
    size_t lineMax;

    void handleInput(int8_t rgbDelta[3], bool knobsPressed[3]);
	
	void switchScreen(Screen* newScreen);
	bool toPreviousScreen(bool keepAlive=false);

    void setColours(uint16_t bgColour, uint16_t fgColour, uint16_t selectColour);
    void setFont(font_descriptor_t font);

	size_t textWidth(std::string& text);

	void clearScreen(uint16_t colour);

	void setPixel(int x, int y, uint16_t color);
	void renderRectangle(int left, int top, int right, int bottom, uint16_t color);
    void renderColourSquare(int topX, int topY, uint16_t colour);
    size_t renderText(int topX, int topY, std::string text, uint16_t colour);
    size_t renderIcon(uint16_t *icon, int topX, int topY, int exponent=0);

    void redraw();

private:
	uint16_t frameBuffer[height][width];
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
    size_t renderCharacter(char character, int topX, int topY, uint16_t colour);

    Mapper mapper = Mapper(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE);

	bool checkBounds(int* x, int* y);

    bool getBit(uint16_t bits, int position);
};
