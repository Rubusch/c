/*
  cunit - tests
 */

#include "dynamic-programming.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


static const int prices[] = { 1, 5, 8, 9, 10, 17, 17, 20, 24, 30 };
//static const int prices[] = { 1, 5, 8 };

void test_cut_rod(void)
{

	int rod_len = sizeof(prices) / sizeof(*prices);

	debug("\n");
	int result = cut_rod(prices, rod_len);
	debug("NAIVE - result %d\n", result);
	CU_ASSERT(30 == result);
}

void test_memoized_cut_rod(void)
{
	int rod_len = sizeof(prices) / sizeof(*prices);
	int result = -1;

	debug("\n");
	result = memoized_cut_rod(prices, rod_len);
	debug("MEMOIZED - result %d\n", result);
	CU_ASSERT(30 == result);
}

void test_bottom_up_cut_rod(void)
{
	int rod_len = sizeof(prices) / sizeof(*prices);
	int result = -1;

	result = bottom_up_cut_rod(prices, rod_len);
	fprintf(stderr, "BOTTOM UP - result %d\n", result);   
	CU_ASSERT(30 == result);
}


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("dynamic-programming", init_suite_dynamic_programming,
			      clean_suite_dynamic_programming);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
/*	TEST_start(pSuite, "memoized rod cut", test_memoized_cut_rod) // */
	TEST_start(pSuite, "rod cut", test_cut_rod)
 		TEST_append(pSuite, "memoized rod cut", test_memoized_cut_rod) // */
/* 		TEST_append(pSuite, "bottom up rod cut", test_bottom_up_cut_rod) // */
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
