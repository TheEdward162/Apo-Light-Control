//
// Created by klara on 14.4.18.
//

#pragma once

#include <stdio.h>

/**
 *  A class that handles the mapping of peripheral physical regions to virtual addresses.
 * */
class Mapper {
public:
    /**
     * Constructor for Mapper.
     * @param base The base region of the peripheral.
     * @param size Address range for the region.
     * */
    Mapper(off_t base, size_t size);
    ~Mapper();

    unsigned char *mem_base = NULL;

private:
    off_t region_base;
    size_t region_size;

    /**
    * The support function which returns pointer to the virtual
    * address at which starts remapped physical region in the
    * process virtual memory space.
     *
     * @param region_base The base region of the peripheral.
     * @param region_size Address range for the region.
     * @param opt_cached Ask God.
    */
    void map_phys_address(off_t region_base, size_t region_size, int opt_cached);

};
