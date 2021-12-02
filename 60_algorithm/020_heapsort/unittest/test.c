/*
  cunit - tests

 */

#include "heapsort.h"

#include "test.h"
#include "stdlib.h"


void print_data(int arr[], int size, int sort_size, int index)
{
#ifdef DEBUG
	fprintf(stderr, "%s(arr, size = %d, sort_size = %d, index = %d):\t %d", __func__, size, sort_size, index, arr[0]);
	for (int idx=1; idx<size; idx++)
		fprintf(stderr, ", %d", arr[idx]);
	fprintf(stderr, "\n");
#endif /* DEBUG */
}

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

/*
  performs heapsort, unrolled and manually
 */
void test_heapify(void)
{
	int arr[10] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0};
	int size = sizeof(arr)/sizeof(int);
	int index = size / 2 - 1;

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

	// performing: build max heap (manually)
	fprintf(stderr, "%s(): max heapify\n", __func__);
	print_data(arr, size, size, 0);
	heapify(arr, size, index);
	print_data(arr, size, size, index);

	index--;
	heapify(arr, size, index);
	print_data(arr, size, size, index);

	index--;
	heapify(arr, size, index);
	print_data(arr, size, size, index);

	index--;
	heapify(arr, size, index);
	print_data(arr, size, size, index);

	index--;
	heapify(arr, size, index);
	print_data(arr, size, size, index);

	CU_ASSERT(9 == arr[0]);
	CU_ASSERT(8 == arr[1]);
	CU_ASSERT(7 == arr[2]);
	CU_ASSERT(6 == arr[3]);
	CU_ASSERT(3 == arr[4]);
	CU_ASSERT(2 == arr[5]);
	CU_ASSERT(4 == arr[6]);
	CU_ASSERT(1 == arr[7]);
	CU_ASSERT(5 == arr[8]);
	CU_ASSERT(0 == arr[9]);

	// heapsort
	fprintf(stderr, "%s(): heapsort\n", __func__);
	int sort_size = size;
	index = size -1;
	swap(&arr[0], &arr[index]);

	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

	swap(&arr[0], &arr[index]);
	sort_size--;
	heapify(arr, index, 0);
	index--;
	print_data(arr, size, sort_size, index);

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

void test_heap(void)
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

	max_heap(arr, size);

	CU_ASSERT(9 == arr[0]);
	CU_ASSERT(8 == arr[1]);
	CU_ASSERT(7 == arr[2]);
	CU_ASSERT(6 == arr[3]);
	CU_ASSERT(3 == arr[4]);
	CU_ASSERT(2 == arr[5]);
	CU_ASSERT(4 == arr[6]);
	CU_ASSERT(1 == arr[7]);
	CU_ASSERT(5 == arr[8]);
	CU_ASSERT(0 == arr[9]);
}

void test_heapsort(void)
{
	int arr[10] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0 };
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

	heapsort(arr, size);

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

void test_insert(void)
{
	int* arr = NULL;
	int size = 10;

	arr = malloc(size * sizeof(*arr));
	if (!arr) {
		CU_ASSERT(FALSE);
		return;
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
	CU_ASSERT(10 == size);

	heapsort(arr, size);

	int key = 23;
	heap_insert(&arr, &size, key);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx=1; idx < size; idx++) {
		fprintf(stderr, ", %d", arr[idx]);
	}
	fprintf(stderr, " - size: %d\n", size);
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
	CU_ASSERT(23 == arr[10]);
	CU_ASSERT(11 == size);

	CU_ASSERT(23 == heap_maximum(arr, size));

	free(arr);
}

void test_extract_max(void)
{
	int* arr = NULL;
	int size = 10;

	arr = malloc(size * sizeof(*arr));
	if (!arr) {
		CU_ASSERT(FALSE);
		return;
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
	CU_ASSERT(10 == size);

	heapsort(arr, size);

	int max_extracted = heap_extract_max(&arr, &size);

#ifdef DEBUG
	fprintf(stderr, "%s(): %d", __func__, arr[0]);
	for (int idx=1; idx < size; idx++) {
		fprintf(stderr, ", %d", arr[idx]);
	}
	fprintf(stderr, " - size: %d\n", size);
#endif /* DEBUG */

	CU_ASSERT(9 == size);
	CU_ASSERT(9 == max_extracted);

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
	TEST_end();

	/* algorithm */
	pSuite = CU_add_suite("heapsort", init_suite_heapsort,
			      clean_suite_heapsort);
	TEST_start(pSuite, "build max heapify", test_heapify)
		TEST_append(pSuite, "build max heap", test_heap)
		TEST_append(pSuite, "heapsort", test_heapsort)
		TEST_append(pSuite, "max heap insert", test_insert)
		TEST_append(pSuite, "heap extract max", test_extract_max)
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
