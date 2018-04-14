//
// Created by klara on 7.4.18.
//

#pragma once

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

    char *map_phys_memdev;
    unsigned char *mem_base = NULL;
    uint32_t knobs_value;
    uint32_t prev_knobs_value;
    char RGBDelta[3];
    bool RGBPressed[3];

    /* Initialize structure to 0 seconds and 200 milliseconds */
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 200 * 1000 * 1000};

        /*
     * The support function which returns pointer to the virtual
     * address at which starts remapped physical region in the
     * process virtual memory space.
     */
    void *map_phys_address(off_t region_base, size_t region_size, int opt_cached);

    //    The main loop
    void handleInput();

    void testDI();

private:
    void update();

    char getDelta(uint8_t prev, uint8_t act);

};
