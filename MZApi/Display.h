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
#include "../Misc/font_types.h"
#include "../MZApi/MZRegisters.h"
#include "../Unit/LightUnit.h"


#define WIDTH 480
#define HEIGHT 320

class Display {
public:
    Display(uint16_t bgColour, uint16_t fgColour, uint16_t selectColour, font_descriptor_t font);
    ~Display();

    void testDisplay();



private:
    uint16_t buffer[HEIGHT][WIDTH];

    uint16_t fgColour, bgColour, selectColour;

    int lineMax;

    font_descriptor_t font;

    void parlcd_write_cmd(uint16_t cmd);

    void parlcd_write_data(uint16_t data);

    Mapper mapper = Mapper(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE);

    void redraw();

    bool getBit(uint16_t bits, int position);

    void renderCharacter(char character, int topX, int topY, uint16_t colour);

    void renderText(int topX, int topY, std::string text, uint16_t colour);

    void renderIcon(uint16_t *buffer, int topX, int topY);

    void renderUnitList(std::vector<LightUnit> units);

    void printDisplay();

};
