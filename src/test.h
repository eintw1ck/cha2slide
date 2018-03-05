#if !defined(__TEST_H)
#define __TEST_H

#include <stdlib.h>
#include "testblake2b.h"
#include "testchacha20.h"
#include "testchunk.h"
#include "testcrc32.h"

static MunitSuite test_suites[] = {
    { (char*) "blake2b/", blake2b_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { (char*) "chacha20/", chacha20_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { (char*) "crc32/", crc32_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { (char*) "chunk/", chunk_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
};

static MunitSuite test_suite = {
  (char*) "cha2slide/",
  NULL,
  test_suites,
  1,
  MUNIT_SUITE_OPTION_NONE
};

#endif
