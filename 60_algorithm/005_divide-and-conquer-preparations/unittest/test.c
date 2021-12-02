/*
  cunit - tests
 */
#include "test.h"
#include "stdlib.h"

void test_sort(void)
{
	const int ARR_SIZE = 50;
	int arr[ARR_SIZE];
	int upper = ARR_SIZE;
	int lower = -1 * ARR_SIZE;
	int idx;

	srand(time(0));
	for (idx=0; idx<ARR_SIZE; idx++) {
		arr[idx] = (rand() % (upper - lower + 1)) + lower;
	}

//*	// DEBUG
	for (idx=0; idx<ARR_SIZE; idx++)
		fprintf(stderr, " %d", arr[idx]);
	fprintf(stderr, "\n");
// */

	content_t* content = malloc(sizeof(content));
	if (!content) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	content->max_left = -1;
	content->max_right = -1;
	content->sum = -1;
	fprintf(stderr, "content: max_left = %d, max_right = %d, sum = %d\n",
		content->max_left, content->max_right, content->sum);

	find_max_crossing_subarray(arr, ARR_SIZE, content);

	fprintf(stderr, "content: max_left = %d {%d}, max_right = %d {%d}, sum = %d\n",
		content->max_left, arr[content->max_left],
		content->max_right, arr[content->max_right], content->sum);

	CU_ASSERT(content->max_left != -1);
	CU_ASSERT(content->max_right != -1);

//*     // DEBUG
	for (idx=0; idx<ARR_SIZE; idx++)
		fprintf(stderr, " %d", arr[idx]);
	fprintf(stderr, "\n");
// */
	free(content);
}

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
	TEST_start(pSuite, "sort", test_sort)
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
