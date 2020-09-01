//
// Created by klara on 14.4.18.
//

#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Display.h"
#include "../Misc/IOTools.h"
#include "../DisplayUtils/Colour.h"
#include "../DisplayUtils/ListScreen.h"
#include "../DisplayUtils/UnitScreen.h"
#include "../DisplayUtils/NyanScreen.h"

#define CHAR_SPACING 10

Display::Display(uint16_t bgColour, uint16_t fgColour, uint16_t selectColour, font_descriptor_t font, DeviceInput* deviceInput) {
    this->bgColour = bgColour;
    this->fgColour = fgColour;
    this->selectColour = selectColour;
    this->deviceInput = deviceInput;
	
	memset(frameBuffer, bgColour, sizeof(uint16_t) * Display::width * Display::height);

	setFont(font);

    currentScreen = new ListScreen(this);

#ifndef MZ_BOARD
	SDL_Init(SDL_INIT_VIDEO);
	sdl_win = SDL_CreateWindow("apo-alc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Display::width, Display::height, SDL_WINDOW_SHOWN);
	SDL_SetWindowTitle(sdl_win, "APO-ALC");
#endif
}

Display::~Display() {
	delete previousScreen;
	delete currentScreen;

#ifndef MZ_BOARD
	SDL_DestroyWindow(sdl_win);
	SDL_Quit();
#endif
}


// call this whenever you want to actually display sth.
void Display::redraw() {
    currentScreen->renderScreen();

#ifdef MZ_BOARD
    parlcd_write_cmd(0x2c);

    for (size_t y = 0; y < Display::height; ++y) {
        for (size_t x = 0; x < Display::width; ++x) {
            parlcd_write_data(frameBuffer[y][x]);
        }
    }
#else
	memset(sdl_knobDeltas, 0, 3 * sizeof(int8_t));
	memset(sdl_knobPresses, false, 3 * sizeof(bool));
	for (SDL_Event event; SDL_PollEvent(&event);) {
        if (event.type == SDL_QUIT)
			exit(0);
		
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				// most intuitive mapping for arrow keys
				case SDLK_UP:
					sdl_knobDeltas[0] = -1;
				break;
				case SDLK_DOWN:
					sdl_knobDeltas[0] = 1;
				break;
				case SDLK_RIGHT:
					sdl_knobPresses[0] = true;
				break;
				case SDLK_LEFT:
					sdl_knobPresses[2] = true;
				break;

				// simulate red knob
				case SDLK_KP_1:
					sdl_knobDeltas[0] = -1;
				break;
				case SDLK_b:
				case SDLK_KP_2:
					sdl_knobPresses[0] = true;
				break;
				case SDLK_KP_3:
					sdl_knobDeltas[0] = 1;
				break;

				// simulate green knob
				case SDLK_KP_4:
					sdl_knobDeltas[1] = -1;
				break;
				case SDLK_n:
				case SDLK_KP_5:
					sdl_knobPresses[1] = true;
				break;
				case SDLK_KP_6:
					sdl_knobDeltas[1] = 1;
				break;

				// simulate blue knob
				case SDLK_KP_7:
					sdl_knobDeltas[2] = -1;
				break;
				case SDLK_m:
				case SDLK_KP_8:
					sdl_knobPresses[2] = true;
				break;
				case SDLK_KP_9:
					sdl_knobDeltas[2] = 1;
				break;
			}
        }
	}

	// use sdl to display the frameBuffer
	SDL_Surface *scr = SDL_GetWindowSurface(sdl_win);
	for (int y = 0; y < scr->h; y++) {
		for (int x = 0; x < scr->w; x++) {
			const int idx = (y * scr->w + x) * scr->format->BytesPerPixel;
			uint8_t *px = (uint8_t*) scr->pixels + idx;

			uint32_t rgb888 = Colour::rgb565to888(frameBuffer[y][x]);

			*(px + scr->format->Rshift / 8) = Colour::getR(rgb888);
			*(px + scr->format->Gshift / 8) = Colour::getG(rgb888);
			*(px + scr->format->Bshift / 8) = Colour::getB(rgb888);
		}
	}
	
	SDL_UpdateWindowSurface(sdl_win);
#endif
}

void Display::setFont(font_descriptor_t font) {
    this->font = font;
    lineMax = (Display::height - 4) / font.height;
}

void Display::setColours(uint16_t bgColour, uint16_t fgColour, uint16_t highlightColour) {
    this->bgColour = bgColour;
    this->fgColour = fgColour;
    this->selectColour = highlightColour;
}

void Display::parlcd_write_data(uint16_t data) {
    *(volatile uint16_t *) (mapper.mem_base + PARLCD_REG_DATA_o) = (uint16_t) data;
}

void Display::parlcd_write_cmd(uint16_t cmd) {
    *(volatile uint16_t *) (mapper.mem_base + PARLCD_REG_CMD_o) = (uint16_t) cmd;
}

bool Display::getBit(uint16_t bits, int position) { // position in range 0-15
    return (bits >> position) & 0x1;
}

bool Display::checkBounds(int* x, int* y) {
	bool changed = false;

	if (*x < 0) {
		*x = 0;
		changed = true;
	} else if (*x >= (int)width) {
		*x = width - 1;
		changed = true;
	}

	if (*y < 0) {
		*y = 0;
		changed = true;
	} else if (*y >= (int)height) {
		*y = height - 1;
		changed = true;
	}

	return changed;
}

void Display::clearScreen(uint16_t colour) {
	uint16_t* b = (uint16_t*)frameBuffer;
	for (size_t i = 0; i < width * height; i++) {
		*(b++) = colour;
	}
}

void Display::setPixel(int x, int y, uint16_t colour) {
	checkBounds(&x, &y);
	frameBuffer[y][x] = colour;
}

void Display::renderRectangle(int left, int top, int right, int bottom, uint16_t colour) {
	checkBounds(&left, &top);
	checkBounds(&right, &bottom);

    for (int rectY = top; rectY < bottom + 1; ++rectY) {
        for (int rectX = left; rectX < right + 1; ++rectX) {
            frameBuffer[rectY][rectX] = colour;
        }
    }
}

void Display::renderColourSquare(int topX, int topY, uint16_t colour) {
	topX++;
	topY++;

	checkBounds(&topX, &topY);
	int bottomX = topX + 14;
	int bottomY = topY + 14;
	checkBounds(&topX, &topY);

    renderRectangle(topX, topY, bottomX, bottomY, colour);
}

// TODO make it general so that it works for the proportional font too
size_t Display::renderCharacter(char character, int topX, int topY, uint16_t colour) {
    checkBounds(&topX, &topY);
	
	int charIndex = font.height * character;
    uint16_t line;
	size_t maxX = 0;

    for (size_t charY = 0; charY < (size_t)font.height; ++charY) {
        line = font.bits[charIndex+charY];
        for (size_t charX = 0; charX < (size_t)font.maxwidth; ++charX) {
            if (getBit(line, 15 - charX)) {
				int bottomX = topX + charX;
				int bottomY = topY + charY;
				checkBounds(&bottomX, &bottomY);

                frameBuffer[bottomY][bottomX] = colour;

				if (charX > maxX)
					maxX = charX;
            }
        }
    }

	if (maxX == 0) {
		// this is probably some whitespace, no touchy
		maxX = 16;
	}

	return maxX;
}

size_t Display::textWidth(std::string& text) {
	return text.length() * CHAR_SPACING;
}

size_t Display::renderText(int topX, int topY, std::string text, uint16_t colour) {
    size_t width = 0;
	
	char character;
    for (size_t i = 0; i < text.size(); ++i) {
        character = text[i];
        renderCharacter(character, topX + width, topY, colour);
		width += CHAR_SPACING;		
    }

	return width;
}

size_t Display::renderIcon(uint16_t *icon, int topX, int topY, int exponent) {
    checkBounds(&topX, &topY);
	
	size_t power = std::pow(2, exponent);
	size_t scaledSize = 16 * power;

	for (size_t iconY = 0; iconY < scaledSize; ++iconY) {
        for (size_t iconX = 0; iconX < scaledSize; ++iconX) {
			size_t iconIndex = iconY / power * 16 + iconX / power;
			setPixel(topX + iconX, topY + iconY, icon[iconIndex]);
        }
    }

	return scaledSize;
}

void Display::handleInput() {
#ifdef MZ_BOARD
 	currentScreen->handleKnobChange(deviceInput->rgbDelta);
    currentScreen->handleKnobPress(deviceInput->knobsPressed);
#else
	currentScreen->handleKnobChange(sdl_knobDeltas);
    currentScreen->handleKnobPress(sdl_knobPresses);
#endif
}

void Display::switchScreen(Screen* newScreen) {
	delete previousScreen;
	previousScreen = currentScreen;
	currentScreen = newScreen;

	deviceInput->lock();
} 
bool Display::toPreviousScreen(bool keepAlive) {
	if (previousScreen != NULL) {
		if (keepAlive) {
			Screen* temp = currentScreen;
			currentScreen = previousScreen;
			previousScreen = temp;
		} else {
			delete currentScreen;
			currentScreen = previousScreen;
			previousScreen = NULL;
		}

		deviceInput->lock();
		return true;
	}

	return false;
}