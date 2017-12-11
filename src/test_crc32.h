#if !defined(__TEST_CRC32_H)
#define __TEST_CRC32_H

#include "munit.h"
#include "crc32.h"

MunitResult test_crc32_gen_lut(const MunitParameter params[], void* user_data);
MunitResult test_crc32(const MunitParameter params[], void* user_data);


static MunitTest crc32_tests[] = {
    { (char*) "gen_lut", test_crc32_gen_lut, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "algorithm", test_crc32, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
};

#endif /* __TEST_CRC32_H */
