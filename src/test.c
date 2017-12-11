#include "test.h"

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
	return munit_suite_main(&test_suite, (void*) "Test", argc, argv);
}
