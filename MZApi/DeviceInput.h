//
// Created by klara on 7.4.18.
//

#ifndef APO_SEM_DEVICEINPUT_H
#define APO_SEM_DEVICEINPUT_H

#define R(a) ((a & 0xFF0000) >> 16)
#define G(a) ((a & 0xFF00) >> 8)
#define B(a) ((a & 0xFF) >> 0)

class DeviceInput {
public:
    DeviceInput();
    ~DeviceInput();

    char *map_phys_memdev;
    unsigned char *mem_base = NULL;
    uint32_t knobs_value;
    uint32_t prev_knobs_value;
    int8_t RGBDelta[3];

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

    void getDelta();

    void testDI();

};



#endif //APO_SEM_DEVICEINPUT_H
