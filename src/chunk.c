#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include "chunk.h"
#include "crc32.h"

static inline uint16_t bswap16(uint16_t x)
{
    return (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8));
}

static inline uint32_t bswap32(uint32_t x)
{
    return (((uint32_t)(bswap16((uint16_t)(x))) << 16) | (uint32_t)(bswap16((uint16_t)(x >> 16))));
}


static inline uint64_t bswap64(uint64_t x)
{
    return (((uint64_t)(bswap32((uint32_t)(x))) << 32) | (uint64_t)(bswap32((uint32_t)(x >> 32))));
}

/**
 * Wrapper function for fwrite to enable on-write calculation of CRCs and bswapping of
 * 16, 32 and 64 bit values.
 */
static size_t fwrite_crc(const void *data, size_t size, FILE *file, uint32_t *crc, int bswap) {
    /* Temporary variables to allow the fwrite call to access memory of the bswap'd value without
     * having to write the bswap'd value to the original memory (thus overwriting a 'read-only'
     * value. */
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    if (!data || !size || !file)
        return 0;

    if (bswap) {
        switch (size) {
        case 1:
            break;
        case 2:
            u16 = bswap16(*(uint16_t*)data);
            data = &u16;
            break;
        case 4:
            u32 = bswap32(*(uint32_t*)data);
            data = &u32;
            break;
        case 8:
            u64 = bswap64(*(uint64_t*)data);
            data = &u64;
            break;
        default:
            fprintf(stderr, "\x1b[31mENOSYS: unsupported bswap-ing %zu bytes.\x1b[0m\n", size);
            exit(EXIT_FAILURE);
        }
    }

    if (crc)
        *crc = crc32_(*crc, data, size);

    return fwrite(data, 1, size, file);
}

#define BEGIN_CHUNK(file, type) \
    int begin = ftell(file); \
    uint32_t crc = DEFAULT_CRC32; \
    fseek(file, 4, SEEK_CUR)

#define END_CHUNK(size) \
    int end = ftell(file); \
    fseek(file, begin, SEEK_SET); \
    fwrite_crc(&size, 4, file, 0, 1); \
    fseek(file, end, SEEK_SET); \
    fwrite_crc(&crc, 4, file, 0, 1)

int png_new_ihdr(png_ihdr **chunk, uint32_t width, uint32_t height, uint8_t bit_depth, uint8_t color_type, uint8_t interlace_method)
{
    if (!(*chunk)) (*chunk) = malloc(sizeof(png_ihdr));
    (*chunk)->type = PNG_IHDR;
    
    if (!((width & 0x7FFFFFFF) && (height & 0x7FFFFFFF)))
        return 1; /* width or height is not a 31 bit integer */
    (*chunk)->width = width;
    (*chunk)->height = height;
    if (bit_depth & (bit_depth - 1))
        return 2; /* bit depth is not a power of two */
    switch (color_type) {
    case 0:
    case 3:
        if (!(bit_depth & 0x000F) && !((color_type == 3) && (bit_depth & 0x0010)))
            return 3; /* bit depth is not 1, 2, 4, or 8 for indexed color or (also 16) for grayscale */
        break;
    case 1:
    case 2:
    case 4:
        if (!(bit_depth & 0x0018)) /* bit depth is not 8 or 16 for grayscale with alpha, truecolor or truecolor with alpha */
            return 4;
        break;
    default: /* color type is unknown */
        return 5;
    }
    (*chunk)->bit_depth = bit_depth;
    (*chunk)->color_type = color_type;
    (*chunk)->filter_method = 0;
    if (interlace_method & 0xFFFE)
        return 6; /* interlace_method is not 1 or 0 */
    (*chunk)->interlace_method = interlace_method;
    
    return 0;
}

int png_new_plte(png_plte **chunk, uint8_t **palette, uint8_t palette_size)
{
    if (!(*chunk)) (*chunk) = malloc(sizeof(png_plte));
    (*chunk)->type = PNG_PLTE;
    
    (*chunk)->palette_size = palette_size;
    (*chunk)->palette = palette;
    
    return 0;
}

int png_set_idat_data(png_idat *chunk, uint8_t *data, uint32_t data_size, uint32_t width) {
    uint32_t height = data_size / (width ? width : 1);

    if (!chunk) return 1;

    chunk->data_size = data_size + height; /* include space for filter type byte for each row */
    chunk->data = malloc(chunk->data_size);

    for (int i = 0; i < (data_size/width); i++) {
        chunk->data[i*(width+1)] = 0;
        memcpy(&chunk->data[1+i*(width+1)], &data[i*width], width);
    }
    return 0;
}

int png_new_idat(png_idat **chunk, uint8_t *data, uint32_t data_size, uint32_t width)
{
    if (!(*chunk)) (*chunk) = malloc(sizeof(png_idat));
    (*chunk)->type = PNG_IDAT;

    (*chunk)->deflate_flag = 1 << 2;

    png_set_idat_data((*chunk), data, data_size, width);

    return 0;
}

int png_new_iend(png_iend **chunk)
{
    if (!(*chunk)) (*chunk) = malloc(sizeof(png_iend));
    (*chunk)->type = PNG_IEND;

    return 0;
}

int png_write_signature(FILE *file)
{
    uint8_t signature[8] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };

    return !(fwrite(signature, 8, 1, file) == 1);
}

int png_write_ihdr(FILE *file, png_ihdr *chunk)
{
    /* Chunk sizes do not include the type, so 4 + 4 + 1 + 1 + 1 + 1 + 1 = 13 */
    uint32_t size = 13;

    BEGIN_CHUNK(file, &chunk->type);
    fwrite_crc(&chunk->type, 4, file, &crc, 1);
    fwrite_crc(&chunk->width, 4, file, &crc, 1);
    fwrite_crc(&chunk->height, 4, file, &crc, 1);
    fwrite_crc(&chunk->bit_depth, 1, file, &crc, 0);
    fwrite_crc(&chunk->color_type, 1, file, &crc, 0);
    fwrite_crc(&chunk->compression_method, 1, file, &crc, 0);
    fwrite_crc(&chunk->filter_method, 1, file, &crc, 0);
    fwrite_crc(&chunk->interlace_method, 1, file, &crc, 0);

    END_CHUNK(size);

    return 0;
}

int png_write_plte(FILE *file, png_plte *chunk)
{
    BEGIN_CHUNK(file, &chunk->type);

    if (chunk->palette_size) {
        for (int i = 0; i < chunk->palette_size/3; i++) {
            fwrite_crc(chunk->palette[i], 3, file, &crc, 0);
        }
    }

    END_CHUNK(chunk->palette_size);

    return 0;
}

int png_write_idat(FILE *file, png_idat *chunk)
{
#define CHUNK 16384
    uint32_t pos = 0;
    uint32_t remaining = chunk->data_size;
    uint8_t out[CHUNK];
    z_stream strm;
    /* Use standard memory allocation functions, this could be hooked up to a different
     * system allocator for embedded systems.
     */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    BEGIN_CHUNK(file, &chunk->type);

    fwrite_crc(&chunk->type, 4, file, &crc, 1);

    /* should use Z_FILTERED as we are doing PNG, but dont actually do filtering so it doesnt do anything */
    int strategy = Z_FILTERED;
    int level    = Z_DEFAULT_COMPRESSION;

    switch (chunk->deflate_flag) {
    case 0: {
        level = Z_NO_COMPRESSION;
        break;
    }
    case 1 << 1: {
        strategy = Z_FIXED;
        break;
    }
    case 1 << 2:
    default: {
        /* default options */
        break;
    }
    }

    if (deflateInit2(&strm, level, Z_DEFLATED, 15, 8, strategy) != Z_OK) {
        fprintf(stderr, "\x1b[31mRIP: %s:%d.\x1b[0m\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    /* while there is data left */
    while (remaining) {
        int flush;
        /* take a chunk of data */
        if (remaining > CHUNK) {
            strm.avail_in = CHUNK;
            flush = Z_NO_FLUSH;
        } else {
            strm.avail_in = remaining;
            flush = Z_FINISH;
        }
        remaining -= strm.avail_in;

        strm.next_in = &chunk->data[pos];
        pos += strm.avail_in;

        strm.avail_out = CHUNK;
        strm.next_out = out;
        if (deflate(&strm, flush) == Z_STREAM_ERROR) {
            fprintf(stderr, "\x1b[31mRIP: %s:%d.\x1b[0m\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
        fwrite_crc(out, CHUNK - strm.avail_out, file, &crc, 0);
    }
    deflateEnd(&strm);

    END_CHUNK(strm.total_out);
    return 0;
}

int png_write_iend(FILE *file, png_iend *chunk)
{
    uint8_t iend[12] = { 0x00, 0x00, 0x00, 0x00, 'I', 'E', 'N', 'D', 0xAE, 0x42, 0x60, 0x82 };
    (void)(chunk);
    return !(fwrite(iend, 12, 1, file) == 1);
}


int png_write_chunk(FILE *file, png_chunk *chunk)
{
    switch (((png_chunk*)chunk)->type) {
        case PNG_IHDR:
            return png_write_ihdr(file, (png_ihdr*)chunk);
        case PNG_PLTE:
            return png_write_plte(file, (png_plte*)chunk);
        case PNG_IDAT:
            return png_write_idat(file, (png_idat*)chunk);
        case PNG_IEND:
            return png_write_iend(file, (png_iend*)chunk);
        default:
            fprintf(stderr, "\x1b[31mEINVAL: unable to write unknown chunk type (%04x).\x1b[0m\n", ((png_chunk*)chunk)->type);
            exit(EXIT_FAILURE);
    }
}

int png_write(FILE *file, png_chunk *chunks[])
{
    png_write_signature(file);
    for (int i = 0, ret = 0; chunks[i]; i++) {
        if ((ret = png_write_chunk(file, chunks[i]))) {
            return ret;
        }
    }
    return 0;
}
