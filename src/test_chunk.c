#include "test_chunk.h"

MunitResult test_write_signature(const MunitParameter params[], void* user_data)
{
    FILE *file = fopen("signature.hex", "wb");
    png_write_signature(file);
    fclose(file);
    return MUNIT_OK;
}

MunitResult test_write_ihdr_chunk(const MunitParameter params[], void* user_data)
{
    FILE *file = fopen("ihdr.hex", "wb");
    png_ihdr *chunk = malloc(sizeof(png_ihdr));
    chunk->type = PNG_IHDR;
    chunk->width = 800;
    chunk->height = 600;
    chunk->bit_depth = 8;
    chunk->color_type = png_color_truecolor_alpha;
    chunk->compression_method = png_compression_method_deflate;
    chunk->filter_method = png_filter_method_0;
    chunk->interlace_method = png_interlace_none;

    png_write_ihdr(file, chunk);
    fclose(file);
    free(chunk);
    return MUNIT_OK;
}

MunitResult test_write_plte_chunk(const MunitParameter params[], void* user_data)
{
    return MUNIT_OK;
}

MunitResult test_write_idat_chunk(const MunitParameter params[], void* user_data)
{
    char data[14] = {
        png_filter0_none,
            0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
        png_filter0_none,
            0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00
    };
    FILE *file = fopen("idat.hex", "wb");
    png_idat *chunk = malloc(sizeof(png_idat));
    chunk->type = PNG_IDAT;

    chunk->zlib_cmf = 0x78;
    chunk->zlib_flag = zlib_fcheck(((uint16_t)chunk->zlib_cmf << 8) + zlib_flevel_default) & 0xFF;
    chunk->deflate_flag = 0;
    
    chunk->data = data;
    chunk->data_size = 14;
    png_write_idat(file, chunk);
    fclose(file);
    free(chunk);
    return MUNIT_OK;
}

MunitResult test_write_iend_chunk(const MunitParameter params[], void* user_data)
{
    FILE *file = fopen("iend.hex", "wb");
    png_iend *chunk = NULL;
    png_write_iend(file, chunk);
    fclose(file);
    return MUNIT_OK;
}

MunitResult test_write_chunks(const MunitParameter params[], void* user_data)
{
    FILE *file = fopen("all.hex", "wb");
    char data[14] = {
        png_filter0_none,
            0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
        png_filter0_none,
            0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00
    };
    png_write_signature(file);

    png_ihdr *ihdr = malloc(sizeof(png_ihdr));
    ihdr->type = PNG_IHDR;
    ihdr->width = 2;
    ihdr->height = 2;
    ihdr->bit_depth = 8;
    ihdr->color_type = png_color_truecolor;
    ihdr->compression_method = png_compression_method_deflate;
    ihdr->filter_method = png_filter_method_0;
    ihdr->interlace_method = png_interlace_none;
    png_write_ihdr(file, ihdr);

    png_idat *idat = malloc(sizeof(png_idat));
    idat->type = PNG_IDAT;

    idat->zlib_cmf = 0x78;
    idat->zlib_flag = zlib_fcheck(((uint16_t)idat->zlib_cmf << 8) + zlib_flevel_default) & 0xFF;
    idat->deflate_flag = 0;

    idat->data = data;
    idat->data_size = 14;
    png_write_idat(file, idat);

    png_iend *iend = malloc(sizeof(png_iend));
    iend->type = PNG_IEND;
    png_write_iend(file, iend);

    free(ihdr);
    free(idat);
    free(iend);
    fclose(file);

    return MUNIT_OK;
}
