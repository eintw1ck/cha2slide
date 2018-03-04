#include "test_crc32.h"
#include <zlib.h>

MunitResult test_crc32_gen_lut(const MunitParameter params[], void* user_data)
{
    crc32_gen_lut();
    return MUNIT_OK;
}

MunitResult test_crc32(const MunitParameter params[], void* user_data)
{
    unsigned char *str1 = (unsigned char*)"123456789";
    unsigned char *str2 = (unsigned char*)"The quick brown fox jumps over the lazy dog.";

    munit_assert_uint(crc32(0, str1, 9),  ==, 0xCBF43926);
    munit_assert_uint(crc32(0, str1, 9),  ==, crc32_(0, str1, 9));
    munit_assert_uint(crc32(0, str2, 44), ==, 0x519025E9);
    munit_assert_uint(crc32(0, str2, 44), ==, crc32_(0, str2, 44));
    return MUNIT_OK;
}
