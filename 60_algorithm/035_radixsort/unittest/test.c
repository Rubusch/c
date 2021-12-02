/*
  cunit - tests

 */

#include "radixsort.h"

#include "test.h"
#include "stdlib.h"


void test_find_max_element(void)
{
	int arr[] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);
	int max_element = find_max_element(arr, size);
	CU_ASSERT(9 == max_element);
}

void test_find_max_element_multiples(void)
{
	int arr[] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0, 3, 7, 4, 6, 5, 0, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);
	int max_element = find_max_element(arr, size);
	CU_ASSERT(9 == max_element);
}

void test_radixsort(void)
{
	int *arr;
	int size = 10;
	arr = malloc(size * sizeof(*arr));
	if (!arr) {
		exit(EXIT_FAILURE);
	}

	arr[0] = 1;
	arr[1] = 92;
	arr[2] = 233;
	arr[3] = 8;
	arr[4] = 32;
	arr[5] = 733;
	arr[6] = 4;
	arr[7] = 62;
	arr[8] = 533;
	arr[9] = 0;

	CU_ASSERT(1 == arr[0]);
	CU_ASSERT(92 == arr[1]);
	CU_ASSERT(233 == arr[2]);
	CU_ASSERT(8 == arr[3]);
	CU_ASSERT(32 == arr[4]);
	CU_ASSERT(733 == arr[5]);
	CU_ASSERT(4 == arr[6]);
	CU_ASSERT(62 == arr[7]);
	CU_ASSERT(533 == arr[8]);
	CU_ASSERT(0 == arr[9]);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	radixsort(&arr, size);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	CU_ASSERT(0 == arr[0]);
	CU_ASSERT(1 == arr[1]);
	CU_ASSERT(4 == arr[2]);
	CU_ASSERT(8 == arr[3]);
	CU_ASSERT(32 == arr[4]);
	CU_ASSERT(62 == arr[5]);
	CU_ASSERT(92 == arr[6]);
	CU_ASSERT(233 == arr[7]);
	CU_ASSERT(533 == arr[8]);
	CU_ASSERT(733 == arr[9]);

	free(arr);
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
	TEST_start(pSuite, "find max element", test_find_max_element)
 		TEST_append(pSuite, "find max element multiple", test_find_max_element_multiples)
	TEST_end();

	/* algorithm */
	pSuite = CU_add_suite("radixsort", init_suite_radixsort,
			      clean_suite_radixsort);
	TEST_start(pSuite, "radixsort", test_radixsort)
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
