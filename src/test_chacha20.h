#if !defined(__TEST_CHACHA20_H)
#define __TEST_CHACHA20_H

#include "munit.h"
#include "chacha20.h"

MunitResult test_chacha20_quarter_round(const MunitParameter params[], void* user_data);
MunitResult test_chacha20_setup(const MunitParameter params[], void* user_data);
MunitResult test_chacha20_block(const MunitParameter params[], void* user_data);
MunitResult test_chacha20_stream(const MunitParameter params[], void* user_data);
MunitResult test_chacha20_encrypt(const MunitParameter params[], void* user_data);

static MunitTest chacha20_tests[] = {
	{ (char*) "quarter_round", test_chacha20_quarter_round, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ (char*) "setup", test_chacha20_setup, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ (char*) "block", test_chacha20_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ (char*) "stream", test_chacha20_stream, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ (char*) "encrypt", test_chacha20_encrypt, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
};



#endif
