/*
  cunit - tests
 */

#ifndef TEST_H
#define TEST_H
# ifndef TRUE
#  define TRUE 1
# endif
# ifndef FALSE
#  define FALSE 0
# endif
#endif

#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"
#include "CUnit/CUCurses.h"

#include "insertionsort.h"

int init_suite_success(void) { return 0; }
int clean_suite_success(void) { return 0; }

void test_swap(void)
{
	int arr[2] = {11, 22};

	CU_ASSERT(11 == arr[0]);
	CU_ASSERT(22 == arr[1]);

	if (-1 == swap(&arr[0], &arr[1]))
		CU_ASSERT(FALSE);

	CU_ASSERT(22 == arr[0]);
	CU_ASSERT(11 == arr[1]);
}

void test_success2(void)
{
	CU_ASSERT(TRUE);
}

// TODO tests



int main()
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_success", init_suite_success,
			      clean_suite_success);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/*add the tests to the suite */
	if (
	   (NULL == CU_add_test(pSuite, "swap", test_swap)) ||
	   (NULL == CU_add_test(pSuite, "successful_test_2", test_success2))
           ) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

//	CU_automated_run_tests();
	CU_curses_run_tests();

	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
