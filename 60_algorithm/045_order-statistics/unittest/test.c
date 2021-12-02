/*
  cunit - tests
 */

#include "statistics.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_utilities_swap(void)
{
	int arr[] = { 1, 2, 3 };

	CU_ASSERT(arr[0] == 1);
	CU_ASSERT(arr[1] == 2);
	CU_ASSERT(arr[2] == 3);

	swap(&arr[0], &arr[2]);

	CU_ASSERT(arr[0] == 3);
	CU_ASSERT(arr[1] == 2);
	CU_ASSERT(arr[2] == 1);

	swap(&arr[1], &arr[0]);

	CU_ASSERT(arr[0] == 2);
	CU_ASSERT(arr[1] == 3);
	CU_ASSERT(arr[2] == 1);
}

void test_find_min_element(void)
{
	int arr[] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);
	int min_element = find_min_element(arr, size);
	CU_ASSERT(0 == min_element);
}

void test_find_max_element(void)
{
	int arr[] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);
	int max_element = find_max_element(arr, size);
	CU_ASSERT(9 == max_element);
}

void test_find_min_element_multiples(void)
{
	int arr[] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0, 3, 7, 4, 6, 5, 0, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);
	int min_element = find_min_element(arr, size);
	CU_ASSERT(0 == min_element);
}

void test_find_max_element_multiples(void)
{
	int arr[] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0, 3, 7, 4, 6, 5, 0, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);
	int max_element = find_max_element(arr, size);
	CU_ASSERT(9 == max_element);
}

void test_median(void)
{
	int arr[] = { 2, 3, 4, 5, 6, 7, 8, 9, 2, 3, 4, 5, 4, 5, 4 };
	int size = sizeof(arr) / sizeof(*arr);
	int median = find_median(arr, size);

	fprintf(stderr, "%s(): median %d\n", __func__, median);
	CU_ASSERT(4 == median);
}

int main()
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("utilities", init_suite_utilities,
			      clean_suite_utilities);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "swap", test_utilities_swap)
 		TEST_append(pSuite, "find min element", test_find_min_element)
 		TEST_append(pSuite, "find max element", test_find_max_element)
 		TEST_append(pSuite, "find min element multiple", test_find_min_element_multiples)
 		TEST_append(pSuite, "find max element multiple", test_find_max_element_multiples)
 		TEST_append(pSuite, "median", test_median)
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
