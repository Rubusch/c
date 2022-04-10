/*
  cunit - tests
 */

#include "get_num.h"

#include "test.h"

#include <stdlib.h>


void test_tlpi_int(void)
{
	long res;

	res = 0;
	CU_ASSERT(0 == res);
	res = get_int("100", GN_NONNEG, "get_int");
	CU_ASSERT(100 == res);

	res = 0;
	CU_ASSERT(0 == res);
	res = get_int("0", GN_NONNEG, "get_int");
	CU_ASSERT(0 == res);

	res = 0;
	CU_ASSERT(0 == res);
	res = get_int("100", GN_GT_0, "get_int");
	CU_ASSERT(100 == res);

// still does not catch failed cases
}


void test_tlpi_long(void)
{
	int res;

	res = 0;
	CU_ASSERT(0 == res);
	res = get_long("100", GN_NONNEG, "get_long");
	CU_ASSERT(100 == res);

	res = 0;
	CU_ASSERT(0 == res);
	res = get_long("0", GN_NONNEG, "get_long");
	CU_ASSERT(0 == res);

	res = 0;
	CU_ASSERT(0 == res);
	res = get_long("100", GN_GT_0, "get_long");
	CU_ASSERT(100 == res);

// still does not catch failed cases
}
// */

int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("dynamic-programming", init_suite_tlpi,
			      clean_suite_tlpi);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "tlpi get_int", test_tlpi_int)
 		TEST_append(pSuite, "tpli get_long", test_tlpi_long) // */
	TEST_end();

	CU_basic_set_mode(CU_BRM_VERBOSE);
#if defined BASICTEST
	CU_basic_run_tests();
#else
	CU_curses_run_tests();
#endif
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

	CU_cleanup_registry();
	return CU_get_error();
}
