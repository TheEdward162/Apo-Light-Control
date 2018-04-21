//
// Created by klara on 14.4.18.
//

#include <cstring>
#include <iostream>
#include <vector>
#include "Display.h"
#include "../Misc/Colour.h"
#include "../Misc/font_rom8x16.h"

Display::Display() {
    memset(buffer, 0, sizeof(uint16_t) * WIDTH * HEIGHT);
    font = font_rom8x16;
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
    int pos_x, pos_y;
    pos_x = 2;
    pos_y = 0;
    std::vector<std::string> text = {"I <3", "APO", "(JUST KIDDING)"};
    for (int i = 0; i < text.size(); ++i) {
        renderText(pos_x, pos_y, text[i], Colour::BLUE);
        pos_y += font.height;
    }


    char tmp;
    // show the result
    for (int y = 0; y < HEIGHT; ++ y) {
        for (int x = 0; x < WIDTH; ++x) {
            tmp = buffer[y][x] > 0 ? '-' : 'x';
            printf("%c", tmp);
        }
        printf("\n");
    }
}

bool Display::getBit(uint16_t bits, int position) // position in range 0-15
{
    return (bits >> position) & 0x1;
}

// TODO make it general so that it works for the proportional font too
void Display::renderCharacter(char character, int top_x, int top_y, uint16_t color) {
    int char_index = font.height*character;
    uint16_t line;
    bool bit;

    for (int char_y = 0; char_y < font.height; ++char_y) {
        line = font.bits[char_index+char_y];
        for (int char_x = 0; char_x < font.maxwidth; ++char_x) {
            bit = getBit(line, 15 - char_x);
            buffer[top_y + char_y][top_x + char_x] = bit;
        }
    }
}

void Display::renderText(int top_x, int top_y, std::string text, uint16_t color) {
    char character;
    for (int i = 0; i < text.size(); ++i) {
        character = text[i];
        renderCharacter(character, top_x, top_y, color);
        top_x += 16;
    }
}
