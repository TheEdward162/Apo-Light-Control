//
// Created by klara on 7.4.18.
//

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cmath>
#include <cstring>
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
}

DeviceInput::~DeviceInput() {}

void DeviceInput::update() {
    uint32_t knobs_value = *(volatile uint32_t*)(mapper.mem_base + SPILED_REG_KNOBS_8BIT_o);

    // get deltas
	memset(rgbDelta, 0, sizeof(int8_t) * 3);
	processDelta(R(knobs_value), 0);
	processDelta(G(knobs_value), 1);
	processDelta(B(knobs_value), 2);
    
    // get presses
    checkLock(RPress(knobs_value), 0);
    checkLock(GPress(knobs_value), 1);
    checkLock(BPress(knobs_value), 2);
}

void DeviceInput::lock() {
    locked[0] = knobsPressed[0];
    locked[1] = knobsPressed[1];
    locked[2] = knobsPressed[2];
}

void DeviceInput::processDelta(uint8_t value, size_t index) {
	// only react to stable positions
	if (value % 4 != 0)
		return;
	
	int16_t distance = (int16_t)value - (int16_t)lastKnobValues[index];
	int8_t sign = std::copysign(1, -distance);
	if (abs(distance) > 128)
		distance = sign * (256 - abs(distance));

	rgbDelta[index] = distance / 4;
	lastKnobValues[index] = value;
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