#if !defined(__TEST_CHUNK_H)
#define __TEST_CHUNK_H

#include "munit.h"
#include "chunk.h"

MunitResult test_write_signature (const MunitParameter params[], void* user_data);
MunitResult test_write_ihdr_chunk(const MunitParameter params[], void* user_data);
MunitResult test_write_plte_chunk(const MunitParameter params[], void* user_data);
MunitResult test_write_idat_chunk(const MunitParameter params[], void* user_data);
MunitResult test_write_iend_chunk(const MunitParameter params[], void* user_data);
MunitResult test_write_chunks(const MunitParameter params[], void* user_data);


static MunitTest chunk_tests[] = {
    { (char*) "write/signature",  test_write_signature, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "write/ihdr_chunk", test_write_ihdr_chunk, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "write/plte_chunk", test_write_plte_chunk, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "write/idat_chunk", test_write_idat_chunk, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "write/iend_chunk", test_write_iend_chunk, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "write/chunks", test_write_chunks, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
};

#endif /* __TEST_CHUNK_H */
