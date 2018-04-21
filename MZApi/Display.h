//
// Created by klara on 14.4.18.
//

#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

#include "Display.h"
#include "Mapper.h"
#include "../Misc/font_types.h"
#include "../MZApi/MZRegisters.h"


#define WIDTH 480
#define HEIGHT 320

class Display {
public:
    Display();
    ~Display();

    void testDisplay();



private:
    font_descriptor_t font;

    uint16_t buffer[HEIGHT][WIDTH];

    void parlcd_write_cmd(uint16_t cmd);

    void parlcd_write_data(uint16_t data);

    Mapper mapper = Mapper(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE);

    void redraw();

    bool getBit(uint16_t bits, int position);

    void renderCharacter(char character, int top_x, int top_y, uint16_t color);

    void renderText(int top_x, int top_y, std::string text, uint16_t color);

};
