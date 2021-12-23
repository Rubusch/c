/*
  cunit - tests
 */

#include "dynamic-programming.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_cut_rod(void)
{
        // NB: indices are shifted by one, i.e. cutting at index '0'
        // has revenue 0
	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30 };
		int expect_max_revenue = 30; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);

		dynamic_programming_debug("\n");
		int result = cut_rod(prices, rod_len);
		dynamic_programming_debug("NAIVE - result %d\n", result);
		CU_ASSERT(expect_max_revenue == result);
	}

	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20 };
		int expect_max_revenue = 22; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);

		dynamic_programming_debug("\n");
		int result = cut_rod(prices, rod_len);
		dynamic_programming_debug("NAIVE - result %d\n", result);
		CU_ASSERT(expect_max_revenue == result);
	}
}

void test_memoized_cut_rod(void)
{
	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30 };
		int expect_max_revenue = 30; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);
		int result = -1;

		dynamic_programming_debug("\n");
		result = memoized_cut_rod(prices, rod_len);
		dynamic_programming_debug("MEMOIZED - result %d\n", result);
		CU_ASSERT(expect_max_revenue == result);
	}

	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20 };
		int expect_max_revenue = 22; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);
		int result = -1;

		dynamic_programming_debug("\n");
		result = memoized_cut_rod(prices, rod_len);
		dynamic_programming_debug("MEMOIZED - result %d\n", result);
		CU_ASSERT(expect_max_revenue == result);
	}
}

void test_bottom_up_cut_rod(void)
{
	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30 };
		int expect_max_revenue = 30; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);
		int result = -1;

		dynamic_programming_debug("\n");
		result = bottom_up_cut_rod(prices, rod_len);
		dynamic_programming_debug("BOTTOM UP - result %d\n", result);
		CU_ASSERT(expect_max_revenue == result);
	}

	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20 };
		int expect_max_revenue = 22; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);
		int result = -1;

		dynamic_programming_debug("\n");
		result = bottom_up_cut_rod(prices, rod_len);
		dynamic_programming_debug("BOTTOM UP - result %d\n", result);
		CU_ASSERT(expect_max_revenue == result);
	}
}

void test_extended_bottom_up_cut_rod(void)
{
	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30 };
		int expect_max_revenue = 30; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);
		int result = -1;

		dynamic_programming_debug("\n");
		result = print_cut_rod_solution(prices, rod_len);
		dynamic_programming_debug("EXTENDED BOTTOM UP - result %d\n",
					  result);
		CU_ASSERT(expect_max_revenue == result);
	}

	{
		int prices[] = { 0, 1, 5, 8, 9, 10, 17, 17, 20 };
		int expect_max_revenue = 22; // maximum obtained revenue
		int rod_len = sizeof(prices) / sizeof(*prices);
		int result = -1;

		dynamic_programming_debug("\n");
		result = print_cut_rod_solution(prices, rod_len);
		dynamic_programming_debug("EXTENDED BOTTOM UP - result %d\n",
					  result);
		CU_ASSERT(expect_max_revenue == result);
	}
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
	TEST_start(pSuite, "rod cut", test_cut_rod)
 		TEST_append(pSuite, "memoized rod cut", test_memoized_cut_rod)
 		TEST_append(pSuite, "bottom up rod cut", test_bottom_up_cut_rod)
 		TEST_append(pSuite, "extended bottom up rod cut", test_extended_bottom_up_cut_rod)
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
