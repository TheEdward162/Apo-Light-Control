//
// Created by klara on 7.4.18.
//

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdint>
#include <time.h>
#include "DeviceInput.h"
#include "../MZApi/MZRegisters.h"


DeviceInput::DeviceInput() {
    knobs_value = 0;
    prev_knobs_value = 0;
}

DeviceInput::~DeviceInput() {}

void DeviceInput::update() {
    prev_knobs_value = knobs_value;
    knobs_value = *(volatile uint32_t*)(mapper.mem_base + SPILED_REG_KNOBS_8BIT_o);

    // get delta
    RGBDelta[0] = getDelta(R(knobs_value), R(prev_knobs_value))/4;
    RGBDelta[1] = getDelta(G(knobs_value), G(prev_knobs_value))/4;
    RGBDelta[2] = getDelta(B(knobs_value), B(prev_knobs_value))/4;
    // get press
    RGBPressed[0] = RPress(knobs_value) ? true : false;
    RGBPressed[1] = GPress(knobs_value) ? true : false;
    RGBPressed[2] = BPress(knobs_value) ? true : false;
}

int8_t DeviceInput::getDelta(uint8_t prev, uint8_t act) {
    // hadnle overflow
    if (prev > 220 && act < 35) {
        return ((int8_t)act - prev + 255);
    }
    // handle underflow
    if (prev < 35 && act > 220) {
        return ((int8_t)act - prev - 255);
    }
    return act - prev;
}
