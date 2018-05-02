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

#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

#define RPress(a) ((a & 0x4000000) >> 26)
#define GPress(a) ((a & 0x2000000) >> 25)
#define BPress(a) ((a & 0x1000000) >> 24)

DeviceInput::DeviceInput() {
    knobs_value = 0;
    prev_knobs_value = 0;
}

DeviceInput::~DeviceInput() {}

void DeviceInput::update() {
    prev_knobs_value = knobs_value;
    knobs_value = *(volatile uint32_t*)(mapper.mem_base + SPILED_REG_KNOBS_8BIT_o);

    // get delta
    rgbDelta[0] = getDelta(R(knobs_value), R(prev_knobs_value)) / -4;
    rgbDelta[1] = getDelta(G(knobs_value), G(prev_knobs_value)) / -4;
    rgbDelta[2] = getDelta(B(knobs_value), B(prev_knobs_value)) / -4;
    
    // get press
    checkLock(RPress(knobs_value), 0);
    checkLock(GPress(knobs_value), 1);
    checkLock(BPress(knobs_value), 2);
}

void DeviceInput::lock() {
    locked[0] = knobsPressed[0];
    locked[1] = knobsPressed[1];
    locked[2] = knobsPressed[2];
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

void DeviceInput::checkLock(bool actual, size_t index) {
    if (locked[index]) {
        if (!actual)
            locked[index] = 0;
        knobsPressed[index] = 0;
    } else {
        knobsPressed[index] = actual;
    }
}