/*
  cunit - tests
 */

#include "matrix.h"

#include "test.h"

#include <stdlib.h>


void test_matrix_simple(void)
{
	matrix_p mat = NULL;
	CU_ASSERT(NULL == mat);
	mat = matrix_create("MATRIX", 10, 10);
	CU_ASSERT(NULL != mat);
	CU_ASSERT(0 == strcmp("MATRIX", mat->name));
	matrix_print(mat);

	CU_ASSERT(0 == mat->m[ 0][ 0]);
	CU_ASSERT(0 == mat->m[ 0][ 1]);
	CU_ASSERT(0 == mat->m[ 0][ 2]);
	CU_ASSERT(0 == mat->m[ 0][ 3]);
	CU_ASSERT(0 == mat->m[ 0][ 4]);
	CU_ASSERT(0 == mat->m[ 0][ 5]);
	CU_ASSERT(0 == mat->m[ 0][ 6]);
	CU_ASSERT(0 == mat->m[ 0][ 7]);
	CU_ASSERT(0 == mat->m[ 0][ 8]);
	CU_ASSERT(0 == mat->m[ 0][ 9]);

	CU_ASSERT(0 == mat->m[ 1][ 0]);
	CU_ASSERT(0 == mat->m[ 1][ 1]);
	CU_ASSERT(0 == mat->m[ 1][ 2]);
	CU_ASSERT(0 == mat->m[ 1][ 3]);
	CU_ASSERT(0 == mat->m[ 1][ 4]);
	CU_ASSERT(0 == mat->m[ 1][ 5]);
	CU_ASSERT(0 == mat->m[ 1][ 6]);
	CU_ASSERT(0 == mat->m[ 1][ 7]);
	CU_ASSERT(0 == mat->m[ 1][ 8]);
	CU_ASSERT(0 == mat->m[ 1][ 9]);

	CU_ASSERT(0 == mat->m[ 2][ 0]);
	CU_ASSERT(0 == mat->m[ 2][ 1]);
	CU_ASSERT(0 == mat->m[ 2][ 2]);
	CU_ASSERT(0 == mat->m[ 2][ 3]);
	CU_ASSERT(0 == mat->m[ 2][ 4]);
	CU_ASSERT(0 == mat->m[ 2][ 5]);
	CU_ASSERT(0 == mat->m[ 2][ 6]);
	CU_ASSERT(0 == mat->m[ 2][ 7]);
	CU_ASSERT(0 == mat->m[ 2][ 8]);
	CU_ASSERT(0 == mat->m[ 2][ 9]);

	CU_ASSERT(0 == mat->m[ 3][ 0]);
	CU_ASSERT(0 == mat->m[ 3][ 1]);
	CU_ASSERT(0 == mat->m[ 3][ 2]);
	CU_ASSERT(0 == mat->m[ 3][ 3]);
	CU_ASSERT(0 == mat->m[ 3][ 4]);
	CU_ASSERT(0 == mat->m[ 3][ 5]);
	CU_ASSERT(0 == mat->m[ 3][ 6]);
	CU_ASSERT(0 == mat->m[ 3][ 7]);
	CU_ASSERT(0 == mat->m[ 3][ 8]);
	CU_ASSERT(0 == mat->m[ 3][ 9]);

	CU_ASSERT(0 == mat->m[ 4][ 0]);
	CU_ASSERT(0 == mat->m[ 4][ 1]);
	CU_ASSERT(0 == mat->m[ 4][ 2]);
	CU_ASSERT(0 == mat->m[ 4][ 3]);
	CU_ASSERT(0 == mat->m[ 4][ 4]);
	CU_ASSERT(0 == mat->m[ 4][ 5]);
	CU_ASSERT(0 == mat->m[ 4][ 6]);
	CU_ASSERT(0 == mat->m[ 4][ 7]);
	CU_ASSERT(0 == mat->m[ 4][ 8]);
	CU_ASSERT(0 == mat->m[ 4][ 9]);

	CU_ASSERT(0 == mat->m[ 5][ 0]);
	CU_ASSERT(0 == mat->m[ 5][ 1]);
	CU_ASSERT(0 == mat->m[ 5][ 2]);
	CU_ASSERT(0 == mat->m[ 5][ 3]);
	CU_ASSERT(0 == mat->m[ 5][ 4]);
	CU_ASSERT(0 == mat->m[ 5][ 5]);
	CU_ASSERT(0 == mat->m[ 5][ 6]);
	CU_ASSERT(0 == mat->m[ 5][ 7]);
	CU_ASSERT(0 == mat->m[ 5][ 8]);
	CU_ASSERT(0 == mat->m[ 5][ 9]);

	CU_ASSERT(0 == mat->m[ 6][ 0]);
	CU_ASSERT(0 == mat->m[ 6][ 1]);
	CU_ASSERT(0 == mat->m[ 6][ 2]);
	CU_ASSERT(0 == mat->m[ 6][ 3]);
	CU_ASSERT(0 == mat->m[ 6][ 4]);
	CU_ASSERT(0 == mat->m[ 6][ 5]);
	CU_ASSERT(0 == mat->m[ 6][ 6]);
	CU_ASSERT(0 == mat->m[ 6][ 7]);
	CU_ASSERT(0 == mat->m[ 6][ 8]);
	CU_ASSERT(0 == mat->m[ 6][ 9]);

	CU_ASSERT(0 == mat->m[ 7][ 0]);
	CU_ASSERT(0 == mat->m[ 7][ 1]);
	CU_ASSERT(0 == mat->m[ 7][ 2]);
	CU_ASSERT(0 == mat->m[ 7][ 3]);
	CU_ASSERT(0 == mat->m[ 7][ 4]);
	CU_ASSERT(0 == mat->m[ 7][ 5]);
	CU_ASSERT(0 == mat->m[ 7][ 6]);
	CU_ASSERT(0 == mat->m[ 7][ 7]);
	CU_ASSERT(0 == mat->m[ 7][ 8]);
	CU_ASSERT(0 == mat->m[ 7][ 9]);

	CU_ASSERT(0 == mat->m[ 8][ 0]);
	CU_ASSERT(0 == mat->m[ 8][ 1]);
	CU_ASSERT(0 == mat->m[ 8][ 2]);
	CU_ASSERT(0 == mat->m[ 8][ 3]);
	CU_ASSERT(0 == mat->m[ 8][ 4]);
	CU_ASSERT(0 == mat->m[ 8][ 5]);
	CU_ASSERT(0 == mat->m[ 8][ 6]);
	CU_ASSERT(0 == mat->m[ 8][ 7]);
	CU_ASSERT(0 == mat->m[ 8][ 8]);
	CU_ASSERT(0 == mat->m[ 8][ 9]);

	CU_ASSERT(0 == mat->m[ 9][ 0]);
	CU_ASSERT(0 == mat->m[ 9][ 1]);
	CU_ASSERT(0 == mat->m[ 9][ 2]);
	CU_ASSERT(0 == mat->m[ 9][ 3]);
	CU_ASSERT(0 == mat->m[ 9][ 4]);
	CU_ASSERT(0 == mat->m[ 9][ 5]);
	CU_ASSERT(0 == mat->m[ 9][ 6]);
	CU_ASSERT(0 == mat->m[ 9][ 7]);
	CU_ASSERT(0 == mat->m[ 9][ 8]);
	CU_ASSERT(0 == mat->m[ 9][ 9]);

	int arr[] = {1,2,3,4,5,6,7,8,9,10};
	int arr_size = sizeof(arr) / sizeof(*arr);
	matrix_init_row(mat, 7, arr, arr_size);
	matrix_print(mat);

	CU_ASSERT(1 == mat->m[ 7][ 0]);
	CU_ASSERT(2 == mat->m[ 7][ 1]);
	CU_ASSERT(3 == mat->m[ 7][ 2]);
	CU_ASSERT(4 == mat->m[ 7][ 3]);
	CU_ASSERT(5 == mat->m[ 7][ 4]);
	CU_ASSERT(6 == mat->m[ 7][ 5]);
	CU_ASSERT(7 == mat->m[ 7][ 6]);
	CU_ASSERT(8 == mat->m[ 7][ 7]);
	CU_ASSERT(9 == mat->m[ 7][ 8]);
	CU_ASSERT(10 == mat->m[ 7][ 9]);

	matrix_destroy(mat);
	mat = NULL;
}

void test_matrix_multiple(void)
{
	matrix_p A = NULL;
	A = matrix_create("A", 3, 2);
	int a0[] = {1,2}; matrix_init_row(A, 0, a0, 2);
	int a1[] = {3,4}; matrix_init_row(A, 1, a1, 2);
	int a2[] = {5,6}; matrix_init_row(A, 2, a2, 2);

	CU_ASSERT(1 == A->m[ 0][ 0]);
	CU_ASSERT(2 == A->m[ 0][ 1]);
	CU_ASSERT(3 == A->m[ 1][ 0]);
	CU_ASSERT(4 == A->m[ 1][ 1]);
	CU_ASSERT(5 == A->m[ 2][ 0]);
	CU_ASSERT(6 == A->m[ 2][ 1]);

	matrix_p B = NULL;
	B = matrix_create("B", 2, 3);
	int b0[] = {1,2,3}; matrix_init_row(B, 0, b0, 3);
	int b1[] = {4,5,6}; matrix_init_row(B, 1, b1, 3);

	CU_ASSERT(1 == B->m[ 0][ 0]);
	CU_ASSERT(2 == B->m[ 0][ 1]);
	CU_ASSERT(3 == B->m[ 0][ 2]);
	CU_ASSERT(4 == B->m[ 1][ 0]);
	CU_ASSERT(5 == B->m[ 1][ 1]);
	CU_ASSERT(6 == B->m[ 1][ 2]);

	matrix_print(A);
	matrix_print(B);

	matrix_destroy(A);
	matrix_destroy(B);
}

void test_matrix_init(void)
{
	matrix_p M = NULL;
	M = matrix_create("M", 3, 3);
	matrix_init_all(M, 123);
	matrix_print(M);

	CU_ASSERT(123 == M->m[ 0][ 0]);
	CU_ASSERT(123 == M->m[ 0][ 1]);
	CU_ASSERT(123 == M->m[ 0][ 2]);
	CU_ASSERT(123 == M->m[ 1][ 0]);
	CU_ASSERT(123 == M->m[ 1][ 1]);
	CU_ASSERT(123 == M->m[ 1][ 2]);
	CU_ASSERT(123 == M->m[ 2][ 0]);
	CU_ASSERT(123 == M->m[ 2][ 1]);
	CU_ASSERT(123 == M->m[ 2][ 2]);
}

int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("dynamic-programming", init_suite_matrix,
			      clean_suite_matrix);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "matrix struct", test_matrix_simple)
 		TEST_append(pSuite, "matrix multiple",
			    test_matrix_multiple) // */
		TEST_append(pSuite, "matrix init",
			    test_matrix_init) // */
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
