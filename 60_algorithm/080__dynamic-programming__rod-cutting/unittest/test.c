/*
  cunit - tests
 */

#include "dynamic-programming.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


static const int prices[] = { 1, 5, 8, 9, 10, 17, 17, 20, 24, 30 };

void test_cut_rod(void)
{

	int nprices = sizeof(prices) / sizeof(*prices);
//	int rod_size = 20; // gets very slow
	int rod_size = 10;

	print_dot_start("cut_rod.dot");
	int result = cut_rod(prices, nprices, rod_size);
	print_dot_end();
//	fprintf(stderr, "XXX res: %d\n", result);   
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
	TEST_start(pSuite, "cut_rod", test_cut_rod)
/* 		TEST_append(pSuite, "empty", test_list_empty) // */
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
