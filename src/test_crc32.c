#include "test_crc32.h"

MunitResult test_crc32_gen_lut(const MunitParameter params[], void* user_data)
{
	crc32_gen_lut();
	return MUNIT_OK;
}

MunitResult test_crc32(const MunitParameter params[], void* user_data)
{
	char *str1 = "123456789";
	char *str2 = "The quick brown fox jumps over the lazy dog.";

	munit_assert_uint(crc32(str1, 9, 0),  ==, 0xCBF43926);
	munit_assert_uint(crc32(str2, 44, 0), ==, 0x519025E9);
	return MUNIT_OK;
}
