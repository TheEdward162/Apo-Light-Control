//
// Created by klara on 14.4.18.
//

#include <cstring>
#include <iostream>
#include <vector>
#include "Display.h"
#include "../Misc/IOTools.h"
#include "../DisplayUtils/ListScreen.h"

Display::Display(uint16_t bgColour_, uint16_t fgColour_, uint16_t selectColour_, font_descriptor_t font_) {
    bgColour = bgColour_;
    fgColour = fgColour_;
    selectColour = selectColour_;
    memset(buffer, bgColour, sizeof(uint16_t) * WIDTH * HEIGHT);

    font = font_;
    lineMax = (HEIGHT - 4)/font.height;

    screen = new ListScreen(this);
}

Display::~Display() {}


// call this whenever you want to actually display sth.
void Display::redraw() {
    screen->renderScreen();

    // do it all again because why not
    parlcd_write_cmd(0x2c);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            parlcd_write_data(buffer[y][x]);
        }
    }
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


void Display::setLightUnits(std::vector<LightUnit>& units) {
    lightUnits = units;
}

void Display::parlcd_write_data(uint16_t data) {
    *(volatile uint16_t *) (PARLCD_REG_BASE_PHYS + PARLCD_REG_DATA_o) = (uint16_t) data;
}

void Display::parlcd_write_cmd(uint16_t cmd) {
    *(volatile uint16_t *) (PARLCD_REG_BASE_PHYS + PARLCD_REG_CMD_o) = (uint16_t) cmd;
}

void Display::testDisplay() {
    screen->renderScreen();
    printDisplay();

}

bool Display::getBit(uint16_t bits, int position) // position in range 0-15
{
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

    for (int charY = 0; charY < font.height; ++charY) {
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
    for (int i = 0; i < text.size(); ++i) {
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

void Display::handleInput(char rgbDelta[3], bool knobsPressed[3]) {
    screen->handleKnobChange(rgbDelta);
    screen->handleKnobPress(knobsPressed);
}
