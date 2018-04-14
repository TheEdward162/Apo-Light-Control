//
// Created by klara on 14.4.18.
//

#pragma once

#include <cstdint>
#include <cstdlib>
#include "Mapper.h"

#define WIDTH 480
#define HEIGHT 320

class Display {
public:
    Display();
    ~Display();

    Mapper mapper;


private:
    uint16_t buffer[WIDTH][HEIGHT];

    void parlcd_write_cmd(uint16_t cmd);

    void parlcd_write_data(uint16_t data);

    void Redraw();
};
