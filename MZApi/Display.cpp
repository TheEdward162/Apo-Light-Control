//
// Created by klara on 14.4.18.
//

#include <cstring>
#include <iostream>
#include <vector>
#include "Display.h"
#include "../Misc/IOTools.h"
#include "../DisplayUtils/ListScreen.h"
#include "../DisplayUtils/Colour.h"

#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

Display::Display(uint16_t bgColour_, uint16_t fgColour_, uint16_t selectColour_, font_descriptor_t font_, std::vector<LightUnit>& lightUnits_) : lightUnits(lightUnits_) {
    bgColour = bgColour_;
    fgColour = fgColour_;
    selectColour = selectColour_;
    memset(buffer, bgColour, sizeof(uint16_t) * WIDTH * HEIGHT);

    font = font_;
    lineMax = (HEIGHT - 4) / font.height;

    screen = new ListScreen(this);

#ifndef MZ_BOARD
	SDL_Init(SDL_INIT_VIDEO);
	sdl_win = SDL_CreateWindow("apo-alc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_SetWindowTitle(sdl_win, "APO-ALC");
#endif
}

Display::~Display() {
#ifndef MZ_BOARD
	SDL_DestroyWindow(sdl_win);
	SDL_Quit();
#endif
}


// call this whenever you want to actually display sth.
void Display::redraw() {
	// clear buffer
    memset(buffer, bgColour, sizeof(uint16_t) * WIDTH * HEIGHT);
    
    screen->renderScreen();

#ifdef MZ_BOARD
    parlcd_write_cmd(0x2c);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            parlcd_write_data(buffer[y][x]);
        }
    }
#else
	memset(sdl_knobDeltas, 0, 3 * sizeof(int8_t));
	memset(sdl_knobPresses, 0, 3 * sizeof(bool));
	for (SDL_Event event; SDL_PollEvent(&event);) {
        if (event.type == SDL_QUIT)
			exit(0);
		
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_UP:
					sdl_knobDeltas[0] = 1;
				break;
				case SDLK_DOWN:
					sdl_knobDeltas[0] = -1;
				break;
				case SDLK_RIGHT:
					sdl_knobPresses[0] = true;
				break;
			}
        }
	}

	// use sdl to display the buffer
	SDL_Surface *scr = SDL_GetWindowSurface(sdl_win);
	for (int y = 0; y < scr->h; y++) {
		for (int x = 0; x < scr->w; x++) {
			const int idx = (y * scr->w + x) * scr->format->BytesPerPixel;
			uint8_t *px = (uint8_t*) scr->pixels + idx;

			uint32_t rgb888 = Colour::rgb565to888(buffer[y][x]);

			*(px + scr->format->Rshift / 8) = R(rgb888);
			*(px + scr->format->Gshift / 8) = G(rgb888);
			*(px + scr->format->Bshift / 8) = B(rgb888);
		}
	}
	
	SDL_UpdateWindowSurface(sdl_win);
#endif
}

void Display::setFont(font_descriptor_t font_) {
    font = font_;
    lineMax = (HEIGHT - 4)/font.height;
}

void Display::setColours(uint16_t bgColour_, uint16_t fgColour_, uint16_t selectColour_) {
    bgColour = bgColour_;
    fgColour = fgColour_;
    selectColour = selectColour_;
}

void Display::parlcd_write_data(uint16_t data) {
    *(volatile uint16_t *) (mapper.mem_base + PARLCD_REG_DATA_o) = (uint16_t) data;
}

void Display::parlcd_write_cmd(uint16_t cmd) {
    *(volatile uint16_t *) (mapper.mem_base + PARLCD_REG_CMD_o) = (uint16_t) cmd;
}

void Display::testDisplay() {
    screen->renderScreen();
    printDisplay();
}

bool Display::getBit(uint16_t bits, int position) { // position in range 0-15
    return (bits >> position) & 0x1;
}

void Display::renderRectangle(int left, int top, int right, int bottom, uint16_t color) {
    for (int rectY = top; rectY < bottom; ++rectY) {
        for (int rectX = left; rectX < right; ++rectX) {
            buffer[rectY][rectX] = color;
        }
    }
}

void Display::renderColourSquare(int topX, int topY, uint16_t colour) {
    renderRectangle(topX + 1, topY + 1, topX + 15, topY + 15, colour);
}

// TODO make it general so that it works for the proportional font too
void Display::renderCharacter(char character, int topX, int topY, uint16_t colour) {
    int charIndex = font.height*character;
    uint16_t line;

    for (size_t charY = 0; charY < font.height; ++charY) {
        line = font.bits[charIndex+charY];
        for (int charX = 0; charX < font.maxwidth; ++charX) {
            if (getBit(line, 15 - charX)) {
                buffer[topY + charY][topX + charX] = colour;
            }
        }
    }
}


void Display::renderText(int topX, int topY, std::string text, uint16_t colour) {
    char character;
    for (size_t i = 0; i < text.size(); ++i) {
        character = text[i];
        renderCharacter(character, topX, topY, colour);
        topX += 16;
    }
}

void Display::renderIcon(uint16_t *buffer_, int topX, int topY) {
    for (int iconY = 0; iconY < 16; ++iconY) {
        for (int icon_x = 0; icon_x < 16; ++icon_x) {
            buffer[topY + iconY][topX + icon_x] = buffer_[iconY * 16 + icon_x];
        }
    }
}

void Display::printDisplay() {
    char tmp;
    // show the result
    for (int y = 0; y < HEIGHT; ++ y) {
        for (int x = 0; x < WIDTH; ++x) {

            if (buffer[y][x] == fgColour) {
                tmp = 'F';
            }
            else {
                if (buffer[y][x] == bgColour) {
                    tmp = '-';
                }
                else {
                    if (buffer[y][x] == selectColour) {
                        tmp = '|';
                    }
                    else {
                        tmp = '?';
                    }
                }
            }
            printf("%c", tmp);
        }
        printf("\n");
    }
    printf("\n\n");
}

void Display::handleInput(int8_t rgbDelta[3], bool knobsPressed[3]) {
#ifdef MZ_BOARD
 	screen->handleKnobChange(rgbDelta);
    screen->handleKnobPress(knobsPressed);
#else
	screen->handleKnobChange(sdl_knobDeltas);
    screen->handleKnobPress(sdl_knobPresses);
#endif
}
