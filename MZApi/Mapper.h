//
// Created by klara on 14.4.18.
//

#pragma once

#include <stdio.h>

class Mapper {
public:
    Mapper(off_t base, size_t size);
    ~Mapper();

    unsigned char *mem_base = NULL;

private:
    off_t region_base;
    size_t region_size;

    /*
    * The support function which returns pointer to the virtual
    * address at which starts remapped physical region in the
    * process virtual memory space.
    */
    void map_phys_address(off_t region_base, size_t region_size, int opt_cached);

};
