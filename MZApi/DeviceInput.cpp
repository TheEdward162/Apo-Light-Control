//
// Created by klara on 7.4.18.
//

#include <stdio.h>
#include <zconf.h>
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

void DeviceInput::handleInput() {

    while (1) {
        /*
         * Access register holding 8 bit relative knobs position
         * The type "(volatile uint32_t*)" casts address obtained
         * as a sum of base address and register offset to the
         * pointer type which target in memory type is 32-bit unsigned
         * integer. The "volatile" keyword ensures that compiler
         * cannot reuse previously read value of the location.
         */
        prev_knobs_value = knobs_value;
        knobs_value = *(volatile uint32_t*)(mapper.mem_base + SPILED_REG_KNOBS_8BIT_o);


        /*
         * Wait for time specified by "loop_delay" variable.
         * Use monotonic clocks as time reference to ensure
         * that wait interval is not prolonged or shortened
         * due to real time adjustment.
         */
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
}

void DeviceInput::update() {
    // get delta

    RGBDelta[0] = getDelta(R(knobs_value), R(prev_knobs_value))/4;
    RGBDelta[1] = getDelta(G(knobs_value), G(prev_knobs_value))/4;
    RGBDelta[2] = getDelta(B(knobs_value), B(prev_knobs_value))/4;
    // get press
    RGBPressed[0] = RPress(knobs_value) ? true : false;
    RGBPressed[1] = GPress(knobs_value) ? true : false;
    RGBPressed[2] = BPress(knobs_value) ? true : false;
}

char DeviceInput::getDelta(uint8_t prev, uint8_t act) {
    // hadnle overflow
    if (prev > 220 && act < 35) {
        return (act - prev + 255);
    }
    // handle underflow
    if (prev < 35 && act > 220) {
        return (act - prev - 255);
    }
    return act - prev;
}

void DeviceInput::testDI() {
    DeviceInput::prev_knobs_value = 0x38FA6FF;
    DeviceInput::knobs_value = 0x38FA502;
    DeviceInput::update();
    printf("Prev: R - %#x, G - %#x, B - %#x\n", R(knobs_value), G(knobs_value), B(knobs_value));
    printf("Act: R - %#x, G - %#x, B - %#x\n", R(prev_knobs_value), G(prev_knobs_value), B(prev_knobs_value));
    printf("Delta: R - %d, G - %d, B - %d\n", RGBDelta[0], RGBDelta[1], RGBDelta[2]);
    printf("Pressed: R - %d, G - %d, B - %d\n", RGBPressed[0], RGBPressed[1], RGBPressed[2]);

}

