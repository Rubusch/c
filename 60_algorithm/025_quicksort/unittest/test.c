/*
  cunit - tests

 */

#include "quicksort.h"

#include "test.h"
#include "stdlib.h"


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

void test_quicksort(void)
{
	int arr[10] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);

	CU_ASSERT(1 == arr[0]);
	CU_ASSERT(9 == arr[1]);
	CU_ASSERT(2 == arr[2]);
	CU_ASSERT(8 == arr[3]);
	CU_ASSERT(3 == arr[4]);
	CU_ASSERT(7 == arr[5]);
	CU_ASSERT(4 == arr[6]);
	CU_ASSERT(6 == arr[7]);
	CU_ASSERT(5 == arr[8]);
	CU_ASSERT(0 == arr[9]);

	quicksort(arr, 0, size-1);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	CU_ASSERT(0 == arr[0]);
	CU_ASSERT(1 == arr[1]);
	CU_ASSERT(2 == arr[2]);
	CU_ASSERT(3 == arr[3]);
	CU_ASSERT(4 == arr[4]);
	CU_ASSERT(5 == arr[5]);
	CU_ASSERT(6 == arr[6]);
	CU_ASSERT(7 == arr[7]);
	CU_ASSERT(8 == arr[8]);
	CU_ASSERT(9 == arr[9]);
}

void test_randomized_quicksort(void)
{
	int arr[10] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);

	CU_ASSERT(1 == arr[0]);
	CU_ASSERT(9 == arr[1]);
	CU_ASSERT(2 == arr[2]);
	CU_ASSERT(8 == arr[3]);
	CU_ASSERT(3 == arr[4]);
	CU_ASSERT(7 == arr[5]);
	CU_ASSERT(4 == arr[6]);
	CU_ASSERT(6 == arr[7]);
	CU_ASSERT(5 == arr[8]);
	CU_ASSERT(0 == arr[9]);

	randomized_quicksort(arr, 0, size-1);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	CU_ASSERT(0 == arr[0]);
	CU_ASSERT(1 == arr[1]);
	CU_ASSERT(2 == arr[2]);
	CU_ASSERT(3 == arr[3]);
	CU_ASSERT(4 == arr[4]);
	CU_ASSERT(5 == arr[5]);
	CU_ASSERT(6 == arr[6]);
	CU_ASSERT(7 == arr[7]);
	CU_ASSERT(8 == arr[8]);
	CU_ASSERT(9 == arr[9]);
}

void test_tail_recursive_quicksort(void)
{
	int arr[10] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);

	CU_ASSERT(1 == arr[0]);
	CU_ASSERT(9 == arr[1]);
	CU_ASSERT(2 == arr[2]);
	CU_ASSERT(8 == arr[3]);
	CU_ASSERT(3 == arr[4]);
	CU_ASSERT(7 == arr[5]);
	CU_ASSERT(4 == arr[6]);
	CU_ASSERT(6 == arr[7]);
	CU_ASSERT(5 == arr[8]);
	CU_ASSERT(0 == arr[9]);

	tail_recursive_quicksort(arr, 0, size-1);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	CU_ASSERT(0 == arr[0]);
	CU_ASSERT(1 == arr[1]);
	CU_ASSERT(2 == arr[2]);
	CU_ASSERT(3 == arr[3]);
	CU_ASSERT(4 == arr[4]);
	CU_ASSERT(5 == arr[5]);
	CU_ASSERT(6 == arr[6]);
	CU_ASSERT(7 == arr[7]);
	CU_ASSERT(8 == arr[8]);
	CU_ASSERT(9 == arr[9]);
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
	TEST_end();

	/* algorithm */
	pSuite = CU_add_suite("quicksort", init_suite_quicksort,
			      clean_suite_quicksort);
	TEST_start(pSuite, "quicksort", test_quicksort)
 		TEST_append(pSuite, "randomized quicksort", test_randomized_quicksort)
 		TEST_append(pSuite, "tail-recursion quicksort", test_tail_recursive_quicksort)
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
