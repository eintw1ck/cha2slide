#include "testchunk.h"

static uint8_t pixel2x2[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00};

MunitResult test_write_signature(const MunitParameter params[], void *user_data)
{
    unsigned char signature_hex[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    unsigned int signature_hex_len = 8;

    char *buffer;
    size_t buffer_len;
    /* fopen("signature.hex", "wb"); */
    FILE *file = open_memstream(&buffer, &buffer_len);

    png_write_signature(file);
    fclose(file);

    munit_assert_uint(signature_hex_len, ==, buffer_len);
    munit_assert_memory_equal(signature_hex_len, signature_hex, buffer);
    free(buffer);

    return MUNIT_OK;
}

MunitResult test_write_ihdr_chunk(const MunitParameter params[], void *user_data)
{
    unsigned char ihdr_hex[] = {0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x03, 0x20, 0x00,
                                0x00, 0x02, 0x58, 0x08, 0x02, 0x00, 0x00, 0x00, 0x15, 0x14, 0x15, 0x27};
    unsigned int ihdr_hex_len = 25;

    char *buffer;
    size_t buffer_len;
    /* fopen("ihdr.hex", "wb"); */
    FILE *file = open_memstream(&buffer, &buffer_len);
    png_ihdr *chunk = NULL;

    munit_assert_false(png_new_ihdr(&chunk, 800, 600, 8, png_color_truecolor, png_interlace_none));

    png_write_ihdr(file, chunk);
    fclose(file);
    free(chunk);

    munit_assert_uint(ihdr_hex_len, ==, buffer_len);
    munit_assert_memory_equal(ihdr_hex_len, ihdr_hex, buffer);
    free(buffer);
    return MUNIT_OK;
}

MunitResult test_write_plte_chunk(const MunitParameter params[], void *user_data)
{
    return MUNIT_OK;
}

MunitResult test_write_idat_chunk(const MunitParameter params[], void *user_data)
{
    /* Generate array with `xxd --include` */
    unsigned char idat_hex[] = {0x00, 0x00, 0x00, 0x15, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x05,
                                0xc1, 0x01, 0x01, 0x00, 0x00, 0x00, 0x80, 0x10, 0xff, 0x4f, 0x17,
                                0x42, 0x50, 0x19, 0x1e, 0xef, 0x04, 0xfc, 0xa4, 0x01, 0x74, 0xf3};
    unsigned int idat_hex_len = 33;

    char *buffer;
    size_t buffer_len;
    /* fopen("idat.hex", "wb"); */
    FILE *file = open_memstream(&buffer, &buffer_len);

    png_idat *chunk = NULL;
    /* check no error */
    munit_assert_false(png_new_idat(&chunk, pixel2x2, 12, 6));

    png_write_idat(file, chunk);

    fseek(file, 0, SEEK_END);
    fclose(file);
    free(chunk->data);
    free(chunk);

    munit_assert_uint(idat_hex_len, ==, buffer_len);
    munit_assert_memory_equal(idat_hex_len, idat_hex, buffer);
    free(buffer);

    return MUNIT_OK;
}

MunitResult test_write_iend_chunk(const MunitParameter params[], void *user_data)
{
    unsigned char iend_hex[] = {0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};
    unsigned int iend_hex_len = 12;

    char *buffer;
    size_t buffer_len;
    /* fopen("iend.hex", "wb"); */
    FILE *file = open_memstream(&buffer, &buffer_len);
    png_iend *chunk = NULL;
    png_write_iend(file, chunk);

    fclose(file);
    free(chunk);

    munit_assert_uint(iend_hex_len, ==, buffer_len);
    munit_assert_memory_equal(iend_hex_len, iend_hex, buffer);
    free(buffer);

    return MUNIT_OK;
}

MunitResult test_write_chunks(const MunitParameter params[], void *user_data)
{
    unsigned char all_png[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49,
                               0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x08, 0x02,
                               0x00, 0x00, 0x00, 0xfd, 0xd4, 0x9a, 0x73, 0x00, 0x00, 0x00, 0x15, 0x49, 0x44,
                               0x41, 0x54, 0x78, 0x9c, 0x05, 0xc1, 0x01, 0x01, 0x00, 0x00, 0x00, 0x80, 0x10,
                               0xff, 0x4f, 0x17, 0x42, 0x50, 0x19, 0x1e, 0xef, 0x04, 0xfc, 0xa4, 0x01, 0x74,
                               0xf3, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};
    unsigned int all_png_len = 78;

    char *buffer;
    size_t buffer_len;
    /* fopen("all.hex", "wb"); */
    FILE *file = open_memstream(&buffer, &buffer_len);

    png_chunk **chunks = calloc(4, sizeof(png_chunk *));
    munit_assert_false(
        png_new_ihdr((png_ihdr **)&chunks[0], 2, 12 / (2 * 3), 8, png_color_truecolor, png_interlace_none));
    munit_assert_false(png_new_idat((png_idat **)&chunks[1], pixel2x2, 12, 2 * 3));
    munit_assert_false(png_new_iend((png_iend **)&chunks[2]));

    png_write(file, chunks);
    fclose(file);

    free(((png_idat *)chunks[1])->data);
    free(chunks[0]);
    free(chunks[1]);
    free(chunks[2]);

    munit_assert_uint(all_png_len, ==, buffer_len);
    munit_assert_memory_equal(all_png_len, all_png, buffer);
    free(buffer);

    return MUNIT_OK;
}
