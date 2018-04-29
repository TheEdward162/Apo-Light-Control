//
// Created by klara on 7.4.18.
//

#pragma once

#include "Mapper.h"
#include "MZRegisters.h"

class DeviceInput {
public:
    DeviceInput();
    ~DeviceInput();

    int8_t RGBDelta[3];
    bool RGBPressed[3];

    void update();

private:
	uint32_t knobs_value;
    uint32_t prev_knobs_value;
    Mapper mapper = Mapper(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE);

    int8_t getDelta(uint8_t prev, uint8_t act);
};
