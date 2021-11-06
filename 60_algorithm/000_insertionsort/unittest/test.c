/*
  cunit - tests
 */
#include "test.h"


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

void test_is_greater(void)
{
	int a = 0;
	int b = 1;
	int c = -1;
	int d = 10;
	int e = -10;

	CU_ASSERT(&a == is_greater(&a, &a));
	CU_ASSERT(&b == is_greater(&b, &a));
	CU_ASSERT(&a == is_greater(&c, &a));
	CU_ASSERT(&d == is_greater(&d, &a));
	CU_ASSERT(&a == is_greater(&e, &a));

	CU_ASSERT(&b == is_greater(&a, &b));
	CU_ASSERT(&b == is_greater(&b, &b));
	CU_ASSERT(&b == is_greater(&c, &b));
	CU_ASSERT(&d == is_greater(&d, &b));
	CU_ASSERT(&b == is_greater(&e, &b));

	CU_ASSERT(&a == is_greater(&a, &c));
	CU_ASSERT(&b == is_greater(&b, &c));
	CU_ASSERT(&c == is_greater(&c, &c));
	CU_ASSERT(&d == is_greater(&d, &c));
	CU_ASSERT(&c == is_greater(&e, &c));

	CU_ASSERT(&d == is_greater(&a, &d));
	CU_ASSERT(&d == is_greater(&b, &d));
	CU_ASSERT(&d == is_greater(&c, &d));
	CU_ASSERT(&d == is_greater(&d, &d));
	CU_ASSERT(&d == is_greater(&e, &d));

	CU_ASSERT(&a == is_greater(&a, &e));
	CU_ASSERT(&b == is_greater(&b, &e));
	CU_ASSERT(&c == is_greater(&c, &e));
	CU_ASSERT(&d == is_greater(&d, &e));
	CU_ASSERT(&e == is_greater(&e, &e));
}

void test_sort(void)
{
	const int ARR_SIZE=50;
	int arr[ARR_SIZE];
	int arr_orig[ARR_SIZE];
	int upper = ARR_SIZE;
	int lower = 0;
	int idx, jdx;

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

	for (idx=1; idx<ARR_SIZE; idx++) {
		CU_ASSERT(arr[idx-1] <= arr[idx]);
		for (jdx=0; jdx<ARR_SIZE; jdx++) {
			if (arr[idx] == arr_orig[jdx]) {
				arr_orig[jdx] = upper+1; // mark "found" by setting an impossible value
				break;
			}
		}
		if (jdx == ARR_SIZE){
			CU_ASSERT(FALSE); // overrun, the element was not in arr_orig, or not that much of that element
		}
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

	/*add the tests to the suite */
	TEST_start(pSuite, "swap", test_swap)
		TEST_append(pSuite, "is_greater", test_is_greater)
		TEST_append(pSuite, "sort", test_sort)
	TEST_end();

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

#if defined BASICTEST
	CU_automated_run_tests();
#else
	CU_curses_run_tests();
#endif

	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
