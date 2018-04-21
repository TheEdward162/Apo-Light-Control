//
// Created by klara on 21.4.18.
//

#pragma once

#include <cstdint>

namespace Colour {
    enum {
        BLACK = 0,
        RED = 0xF800,
        YELLOW = 0xFF80,
        LIME = 0xB7E0,
        GREEN = 0x4FE0,
        DARK_GREEN = 0x4D24,
        BLUE = 0x051F,
        PURPLE = 0x881F,
        BROWN = 0x9260
    };

    uint16_t rgb888to565(uint32_t rgb888);
};
