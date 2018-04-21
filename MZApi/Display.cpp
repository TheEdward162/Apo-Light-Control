//
// Created by klara on 14.4.18.
//

#include <cstring>
#include <iostream>
#include <vector>
#include "Display.h"
#include "../Misc/IOTools.h"

Display::Display(uint16_t bgColour_, uint16_t fgColour_, uint16_t selectColour_, font_descriptor_t font_) {
    font = font_;
    bgColour = bgColour_;
    fgColour = fgColour_;
    selectColour = selectColour_;
    lineMax = (HEIGHT - 4)/font.height;
    memset(buffer, bgColour, sizeof(uint16_t) * WIDTH * HEIGHT);
}

Display::~Display() {}

void Display::redraw() {
    //CLean display
    parlcd_write_cmd(0x2c);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            parlcd_write_data(buffer[y][x]);
        }
    }
}

void Display::parlcd_write_data(uint16_t data) {
    *(volatile uint16_t *) (PARLCD_REG_BASE_PHYS + PARLCD_REG_DATA_o) = (uint16_t) data;
}

void Display::parlcd_write_cmd(uint16_t cmd) {
    *(volatile uint16_t *) (PARLCD_REG_BASE_PHYS + PARLCD_REG_CMD_o) = (uint16_t) cmd;
}

void Display::testDisplay() {
    std::vector<LightUnit> lightUnits = {};
    char tmp_string[16];
    // create light units
    for (int i = 1; i < 8; ++i) {
        sprintf(tmp_string, "LightUnit #%d", i + 1);
        lightUnits.push_back(LightUnit(tmp_string));
        sprintf(tmp_string, "icons/%d.ppm", i%7);
        IOTools::loadImage16x16(tmp_string, lightUnits[i].image);
    }

    IOTools::loadImage16x16("icons/heart.ppm", lightUnits[1].image);

    renderUnitList(lightUnits);
    printDisplay();

}

bool Display::getBit(uint16_t bits, int position) // position in range 0-15
{
    return (bits >> position) & 0x1;
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
        for (int iconX = 0; iconX < 16; ++iconX) {
            buffer[topY + iconY][topX + iconX] = buffer_[iconY * 16 + iconX];
        }
    }
}

void Display::renderUnitList(std::vector<LightUnit> units) {
    LightUnit unit;
    int y = 2;
    int x = 2;
    for (int i = 0; i < units.size(); ++i) {
        if (i == lineMax) {
            break;
        }
        x = 0;
        unit = units[i];
        renderIcon(unit.image, x, y);
        x+= 18;
        renderText(x, y, unit.description, fgColour);
        y += 16;
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
                        tmp = 'S';
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