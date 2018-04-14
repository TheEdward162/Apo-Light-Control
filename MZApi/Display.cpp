//
// Created by klara on 14.4.18.
//

#include <cstring>
#include "Display.h"
#include "MZRegisters.h"

void Display::Display() {
    mapper = Mapper();
    mapper.map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
}

void Display::Redraw() {
    //CLean display
    parlcd_write_cmd(0x2c);
    memset(buffer, 0, sizeof(uint16_t) * WIDTH * HEIGHT);

    for (int y = 0; y < 320; ++y) {
        for (int x = 0; x < 480; ++x) {
            parlcd_write_data(buffer[x][y]);
        }
    }
}

void Display::parlcd_write_data(uint16_t data) {
    *(volatile uint16_t *) (PARLCD_REG_BASE_PHYS + PARLCD_REG_DATA_o) = (uint16_t) data;
}

void Display::parlcd_write_cmd(uint16_t cmd) {
    *(volatile uint16_t *) (PARLCD_REG_BASE_PHYS + PARLCD_REG_CMD_o) = (uint16_t) cmd;
}