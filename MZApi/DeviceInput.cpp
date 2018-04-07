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
        // TODO uncomment this when ready
//    map_phys_memdev = (char *) "/dev/mem";
//    mem_base = (unsigned char *) map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
//    if (mem_base == NULL)
//        exit(1);
}

DeviceInput::~DeviceInput() {}

void *DeviceInput::map_phys_address(off_t region_base, size_t region_size, int opt_cached)
{
    unsigned long mem_window_size;
    unsigned long pagesize;
    unsigned char *mm;
    unsigned char *mem;
    int fd;

    /*
     * Open a device ("/dev/mem") representing physical address space
     * in POSIX systems
     */
    fd = open(map_phys_memdev, O_RDWR | (!opt_cached? O_SYNC: 0));
    if (fd < 0) {
        fprintf(stderr, "cannot open %s\n", map_phys_memdev);
        return NULL;
    }

    /*
     * The virtual to physical address mapping translation granularity
     * corresponds to memory page size. This call obtains the page
     * size used by running operating system at given CPU architecture.
     * 4kB are used by Linux running on ARM, ARM64, x86 and x86_64 systems.
     */
    pagesize=sysconf(_SC_PAGESIZE);

    /*
     * Extend physical region start address and size to page size boundaries
     * to cover complete requested region.
     */
    mem_window_size = ((region_base & (pagesize-1)) + region_size + pagesize-1) & ~(pagesize-1);

    /*
     * Map file (in our case physical memory) range at specified offset
     * to virtual memory ragion/area (see VMA Linux kernel structures)
     * of the process.
     */
    mm = (unsigned char *) mmap(NULL, mem_window_size, PROT_WRITE|PROT_READ,
              MAP_SHARED, fd, region_base & ~(pagesize-1));

    /* Report failure if the mmap is not allowed for given file or its region */
    if (mm == MAP_FAILED) {
        fprintf(stderr,"mmap error\n");
        return NULL;
    }

    /*
     * Add offset in the page to the returned pointer for non-page-aligned
     * requests.
     */
    mem = mm + (region_base & (pagesize-1));

    return mem;
}


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
        knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);


        /*
         * Wait for time specified by "loop_delay" variable.
         * Use monotonic clocks as time reference to ensure
         * that wait interval is not prolonged or shortened
         * due to real time adjustment.
         */
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
}

void DeviceInput::getDelta() {
    RGBDelta[0] = R(knobs_value) - R(prev_knobs_value);
    RGBDelta[1] = G(knobs_value) - G(prev_knobs_value);
    RGBDelta[2] = B(knobs_value) - B(prev_knobs_value);
}

void DeviceInput::testDI() {
    DeviceInput::prev_knobs_value = 0xAA8FA601;
    DeviceInput::knobs_value = 0xBB8FA501;
    DeviceInput::getDelta();
    printf("Prev: R - %#x, G - %#x, B - %#x\n", R(knobs_value), G(knobs_value), B(knobs_value));
    printf("Act: R - %#x, G - %#x, B - %#x\n", R(prev_knobs_value), G(prev_knobs_value), B(prev_knobs_value));
    printf("Delta: R - %d, G - %d, B - %d\n", RGBDelta[0], RGBDelta[1], RGBDelta[2]);

}

