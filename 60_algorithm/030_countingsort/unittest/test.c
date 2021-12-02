/*
  cunit - tests

 */

#include "countingsort.h"

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

void test_countingsort(void)
{
	int *arr;
	int size = 10;
	arr = malloc(size * sizeof(*arr));
	if (!arr) {
		exit(EXIT_FAILURE);
	}

	arr[0] = 1;
	arr[1] = 9;
	arr[2] = 2;
	arr[3] = 8;
	arr[4] = 3;
	arr[5] = 7;
	arr[6] = 4;
	arr[7] = 6;
	arr[8] = 5;
	arr[9] = 0;

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

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	int min_element, max_element;
	min_element = find_min_element(arr, size);
	max_element = find_max_element(arr, size);

#ifdef DEBUG
	fprintf(stderr, "%s(): min_element %d\n", __func__, min_element);
	fprintf(stderr, "%s(): max_element %d\n", __func__, max_element);
#endif /* DEBUG */

	countingsort(&arr, size, min_element, max_element);

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

	free(arr);
}

void test_countingsort_multiples(void)
{
	int *arr;
	int size = 18;
	arr = malloc(size * sizeof(*arr));
	if (!arr) {
		exit(EXIT_FAILURE);
	}

	arr[ 0] = 1;
	arr[ 1] = 9;
	arr[ 2] = 2;
	arr[ 3] = 8;
	arr[ 4] = 3;
	arr[ 5] = 7;
	arr[ 6] = 4;
	arr[ 7] = 6;
	arr[ 8] = 5;
	arr[ 9] = 0;
	arr[10] = 7;
	arr[11] = 4;
	arr[12] = 6;
	arr[13] = 5;
	arr[14] = 0;
	arr[15] = 6;
	arr[16] = 5;
	arr[17] = 0;

	CU_ASSERT(1 == arr[ 0]);
	CU_ASSERT(9 == arr[ 1]);
	CU_ASSERT(2 == arr[ 2]);
	CU_ASSERT(8 == arr[ 3]);
	CU_ASSERT(3 == arr[ 4]);
	CU_ASSERT(7 == arr[ 5]);
	CU_ASSERT(4 == arr[ 6]);
	CU_ASSERT(6 == arr[ 7]);
	CU_ASSERT(5 == arr[ 8]);
	CU_ASSERT(0 == arr[ 9]);
	CU_ASSERT(7 == arr[10]);
	CU_ASSERT(4 == arr[11]);
	CU_ASSERT(6 == arr[12]);
	CU_ASSERT(5 == arr[13]);
	CU_ASSERT(0 == arr[14]);
	CU_ASSERT(6 == arr[15]);
	CU_ASSERT(5 == arr[16]);
	CU_ASSERT(0 == arr[17]);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	int min_element, max_element;
	min_element = find_min_element(arr, size);
	max_element = find_max_element(arr, size);

#ifdef DEBUG
	fprintf(stderr, "%s(): min_element %d\n", __func__, min_element);
	fprintf(stderr, "%s(): max_element %d\n", __func__, max_element);
#endif /* DEBUG */

	countingsort(&arr, size, min_element, max_element);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx = 1; idx < size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */

	CU_ASSERT(0 == arr[ 0]);
	CU_ASSERT(0 == arr[ 1]);
	CU_ASSERT(0 == arr[ 2]);
	CU_ASSERT(1 == arr[ 3]);
	CU_ASSERT(2 == arr[ 4]);
	CU_ASSERT(3 == arr[ 5]);
	CU_ASSERT(4 == arr[ 6]);
	CU_ASSERT(4 == arr[ 7]);
	CU_ASSERT(5 == arr[ 8]);
	CU_ASSERT(5 == arr[ 9]);
	CU_ASSERT(5 == arr[10]);
	CU_ASSERT(6 == arr[11]);
	CU_ASSERT(6 == arr[12]);
	CU_ASSERT(6 == arr[13]);
	CU_ASSERT(7 == arr[14]);
	CU_ASSERT(7 == arr[15]);
	CU_ASSERT(8 == arr[16]);
	CU_ASSERT(9 == arr[17]);

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
	TEST_start(pSuite, "swap", test_utilities_swap)
 		TEST_append(pSuite, "find min element", test_find_min_element)
 		TEST_append(pSuite, "find max element", test_find_max_element)
 		TEST_append(pSuite, "find min element multiple", test_find_min_element_multiples)
 		TEST_append(pSuite, "find max element multiple", test_find_max_element_multiples)
	TEST_end();

	/* algorithm */
	pSuite = CU_add_suite("countingsort", init_suite_countingsort,
			      clean_suite_countingsort);
	TEST_start(pSuite, "countingsort", test_countingsort)
 		TEST_append(pSuite, "countingsort multiples", test_countingsort_multiples)
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
