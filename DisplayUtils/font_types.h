/*******************************************************************

  font_types.h      - simple bitmap fonts type definition

  Simplified font type descriptor based on
  Microwindows/Nano-X library by Greg Haerr

    https://github.com/ghaerr/microwindows

  Copyright (c) 1999, 2000, 2001, 2002, 2003, 2005, 2010, 2011 Greg Haerr <greg@censoft.com>
  Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.

  Simplification by Pavel Pisa for Czech Technical University
  Computer Architectures course

 *******************************************************************/

#ifndef FONT_TYPES_H
#define FONT_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t font_bits_t;

/* builtin C-based proportional/fixed font structure*/
/**
	@brief Bitmap font struct
*/
typedef struct {
		/** @brief font name*/
        char * name;
		/** @brief max width in pixels*/
        int maxwidth;
	 	/** @brief height in pixels*/
        unsigned int height;
        /** @brief ascent (baseline) height*/
		int ascent;
		/** @brief first character in bitmap*/
        int firstchar;
        /** @brief font size in characters*/
		int size;
		/** @brief 16-bit right-padded bitmap data*/
        const font_bits_t *bits;
		/** @brief offsets into bitmap data*/
        const uint32_t  *offset;
		/** @brief character widths or 0 if fixed*/
        const unsigned char *width;
        /** @brief default char (not glyph index)*/
		int defaultchar;
		/** @brief # words of MWIMAGEBITS bits*/
        int32_t bits_size;
} font_descriptor_t;

extern font_descriptor_t font_prop14x16;

extern font_descriptor_t font_rom8x16;

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif  /*FONT_TYPES_H*/
