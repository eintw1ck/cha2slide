#if !defined(__TEST_BLAKE2B_H)
#define __TEST_BLAKE2B_H

#include "blake2b.h"
#include "munit.h"

MunitResult test_blake2b(const MunitParameter params[], void *user_data);

static MunitTest blake2b_tests[] = {
    {(char *)"algorithm", test_blake2b, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

#endif /* __TEST_BLAKE2B_H */
