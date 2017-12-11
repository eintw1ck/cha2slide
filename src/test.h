#if !defined(__TEST_H)
#define __TEST_H

#include <stdlib.h>
#include "test_blake2b.h"
#include "test_chacha20.h"
#include "test_chunk.h"
#include "test_crc32.h"

static const MunitSuite test_suites[] = {
    { (char*) "blake2b/", blake2b_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { (char*) "chacha20/", chacha20_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { (char*) "crc32/", crc32_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { (char*) "chunk/", chunk_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE },
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
};

static const MunitSuite test_suite = {
  (char*) "",
  NULL,
  test_suites,
  1,
  MUNIT_SUITE_OPTION_NONE
};

#endif
