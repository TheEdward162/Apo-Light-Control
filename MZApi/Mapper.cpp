//
// Created by klara on 14.4.18.
//


#include <stdio.h>
#include <sys/mman.h>
#include "zconf.h"
#include <fcntl.h>
#include <cstdlib>
#include <stdexcept>

#include "Mapper.h"

Mapper::Mapper(off_t base, size_t size) {
    region_base = base;
    region_size = size;

#ifdef MZ_BOARD
    map_phys_address(region_base, region_size, 0);
#endif
}

Mapper::~Mapper() {}

void Mapper::map_phys_address(off_t region_base, size_t region_size, int opt_cached) {
        unsigned long mem_window_size;
        unsigned long pagesize;
        unsigned char *mm;
        unsigned char *mem;
        int fd;

        /*
         * Open a device ("/dev/mem") representing physical address space
         * in POSIX systems
         */
        fd = open("/dev/mem", O_RDWR | (!opt_cached? O_SYNC: 0));
        if (fd < 0) {
			throw std::runtime_error("Could not open /dev/mem");
        }

        /*
         * The virtual to physical address mapping translation granularity
         * corresponds to memory page size. This call obtains the page
         * size used by running operating system at given CPU architecture.
         * 4kB are used by Linux running on ARM, ARM64, x86 and x86_64 systems.
         */
        pagesize = sysconf(_SC_PAGESIZE);

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
            throw std::runtime_error("Could not mmap");
        }

        /*
         * Add offset in the page to the returned pointer for non-page-aligned
         * requests.
         */
        mem = mm + (region_base & (pagesize-1));

       mem_base = mem;
}