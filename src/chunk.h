#if !defined(__CHUNK_H)
#define __CHUNK_H

#include <stdint.h>
#include <stdio.h>

#include "macro.h"

#define PNG_IHDR DWORD('I','H','D','R')
#define PNG_PLTE DWORD('P','L','T','E')
#define PNG_IDAT DWORD('I','D','A','T')
#define PNG_IEND DWORD('I','E','N','D')

static uint32_t png_chunks_supported[] = {
    PNG_IHDR,
    PNG_PLTE,
    PNG_IDAT,
    PNG_IEND,
    0,
};

/**
 * All chunks consist of 3 or 4 sections:
 * [ LENGTH ]: The 32 bit unsigned integer equal to or smaller than 2^31-1 denoting the CHUNK DATA size.
 * [ CHUNK TYPE ]: The 32 bit value in which each byte is an ascii value restricted to the uppercase and lowercase ISO 646 letters.
 * [ CHUNK DATA ]: The chunk data, this can be empty if the chunk type has no data such as IEND chunk.
 * [ CRC ]: The 32 bit CRC value of all the CHUNK TYPE and CHUNK DATA but not the LENGTH.
 */

typedef struct png_chunk
{
    uint32_t type;
} png_chunk;

/**
 * @breif Image header
 */
typedef struct png_ihdr
{
    uint32_t type;
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
} png_ihdr;


enum png_color_type
{
    png_color_grayscale = 0,
    png_color_truecolor = 2,
    png_color_indexed_color = 3,
    png_color_greyscale_alpha = 4,
    png_color_truecolor_alpha = 6,
};

enum png_compression_method
{
    png_compression_method_deflate = 0,
};

enum png_filter_method
{
    png_filter_method_0 = 0,
};

enum png_filter0
{
    png_filter0_none = 0,
    png_filter0_sub = 1,
    png_filter0_up = 2, 
    png_filter0_average = 3,
    png_filter0_paeth = 4,
};

enum png_interlace_method
{
    png_interlace_none = 0,
    png_interlace_adam7 = 1,
};

/**
 * @breif Palette
 */
typedef struct png_plte
{
    uint32_t type;
    uint8_t **palette; 
    /** The palette size must be divisible by 3 so that palette can be 
     *  cast to uint8_t[palette_size/3][3] as it is an array of 24bit
     *  rgb values.
     */
    uint8_t palette_size; 
} png_plte;

/**
 * @breif Image Data
 */
typedef struct png_idat
{
    uint32_t type;
    uint8_t deflate_flag; /* 0b0.00.00000  */
    uint8_t *data;
    uint32_t data_size;
} png_idat;

/**
 * @breif Image trailer
 */
typedef    struct png_iend
{
    uint32_t type;
} png_iend;

int png_new_ihdr(png_ihdr **chunk, uint32_t width, uint32_t height, uint8_t bit_depth, uint8_t color_type, uint8_t interlace_method);
int png_new_plte(png_plte **chunk, uint8_t *palette[3], uint8_t palette_size);
int png_new_idat(png_idat **chunk, uint8_t *data, uint32_t data_size, uint32_t width);
int png_new_iend(png_iend **chunk);

int png_write_signature(FILE *file);
int png_write_ihdr(FILE *file, png_ihdr *chunk);
int png_write_plte(FILE *file, png_plte *chunk);
int png_write_idat(FILE *file, png_idat *chunk);
int png_write_iend(FILE *file, png_iend *chunk);

int png_write_chunk(FILE *file, png_chunk *chunk);
int png_write(FILE *file, png_chunk *chunks[]);

#endif
