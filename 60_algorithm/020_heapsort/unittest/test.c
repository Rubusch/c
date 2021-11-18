/*
  cunit - tests

 */

#include "heapsort.h"

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

	print_structure(arr, sizeof(arr)/sizeof(int), "test.dot");
}


void test_heapify(void)
{
	int arr[10] = { 1, 9, 2, 8, 3, 7, 4, 6, 5 };
	int size = sizeof(arr)/sizeof(int);

//	print_structure(arr, sizeof(arr)/sizeof(int), "heapify00.dot");
	heapify(arr, size, 1);
//	print_structure(arr, sizeof(arr)/sizeof(int), "heapify01.dot");

	// TODO
}

void test_heapsort(void)
{
	int arr[10] = { 1, 9, 2, 8, 3, 7, 4, 6, 5, 0 };
	int size = sizeof(arr)/sizeof(int);

	heapsort(arr, size);

	print_structure(arr, size, "heapsort.dot");

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
/*		TEST_append(pSuite, "matrix combine", test_matrix_combine) //  */
	TEST_end();

	/* algorithm */
	pSuite = CU_add_suite("heapsort", init_suite_heapsort,
			      clean_suite_heapsort);
	TEST_start(pSuite, "heapify", test_heapify)
		TEST_append(pSuite, "heapsort", test_heapsort)
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
