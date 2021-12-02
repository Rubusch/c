/*
  cunit - tests
 */
#include "test.h"

int compare(const void* left, const void* right)
{
	return (int) *(int*) left - *(int*) right;
}
void test_sort(void)
{
	const int ARR_SIZE=50;
	int arr[ARR_SIZE];
	int arr_orig[ARR_SIZE];
	int upper = ARR_SIZE;
	int lower = 0;
	int idx;

	srand(time(0));

	for (idx=0; idx<ARR_SIZE; idx++) {
		arr[idx] = (rand() % (upper - lower + 1)) + lower;
		arr_orig[idx] = arr[idx];
	}
//*	// DEBUG
	for (idx=0; idx<ARR_SIZE; idx++)
		fprintf(stderr, " %d", arr[idx]);
	fprintf(stderr, "\n");
// */

	sort(arr, ARR_SIZE);

	qsort(arr_orig, ARR_SIZE, sizeof(int), compare);
	for (idx=1; idx<ARR_SIZE; idx++) {
		CU_ASSERT(arr[idx] == arr_orig[idx]);
	}

//*     // DEBUG
	for (idx=0; idx<ARR_SIZE; idx++)
		fprintf(stderr, " %d", arr[idx]);
	fprintf(stderr, "\n");
// */
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

	/* add the tests to the suite */
	TEST_start(pSuite, "insertionsort", test_sort)
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

	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
