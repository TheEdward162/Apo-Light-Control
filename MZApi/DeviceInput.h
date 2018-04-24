//
// Created by klara on 7.4.18.
//

#pragma once

#include "Mapper.h"
#include "../MZApi/MZRegisters.h"


#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

#define RPress(a) ((a & 0x4000000) >> 26)
#define GPress(a) ((a & 0x2000000) >> 25)
#define BPress(a) ((a & 0x1000000) >> 24)


class DeviceInput {
public:
    DeviceInput();
    ~DeviceInput();

    uint32_t knobs_value;
    uint32_t prev_knobs_value;
    char RGBDelta[3];
    bool RGBPressed[3];

    /* Initialize structure to 0 seconds and 200 milliseconds */
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 200 * 1000 * 1000};

    //    The main loop
    void handleInput();

    void testDI();

private:
    Mapper mapper = Mapper(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE);

    void update();

    char getDelta(uint8_t prev, uint8_t act);

};
