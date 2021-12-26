/*
  cunit - tests
 */

#include "dynamic-programming.h"
#include "matrix.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_matrix(void)
{
	matrix_p mat = NULL;
	CU_ASSERT(NULL == mat);
	mat = matrix_create("MATRIX", 10, 10);
	CU_ASSERT(NULL != mat);
	matrix_print(mat);
	int arr[] = {1,2,3,4,5,6,7,8,9,10};
	int arr_size = sizeof(arr) / sizeof(*arr);
	matrix_init_row(mat, 7, arr, arr_size);
	matrix_print(mat);
	matrix_destroy(mat);
	mat = NULL;
}

void test_matrix_multiply(void)
{
	{
		matrix_p A = NULL;
		A = matrix_create("A", 3, 3);
		int a0[] = {1,2,3}; matrix_init_row(A, 0, a0, 3);
		int a1[] = {4,5,6}; matrix_init_row(A, 1, a1, 3);
		int a2[] = {7,8,9}; matrix_init_row(A, 2, a2, 3);

		matrix_p B = NULL;
		B = matrix_create("B", 3, 3);
		int b0[] = {9,8,7}; matrix_init_row(B, 0, b0, 3);
		int b1[] = {6,5,4}; matrix_init_row(B, 1, b1, 3);
		int b2[] = {3,2,1}; matrix_init_row(B, 2, b2, 3);

		matrix_p C = NULL;
		C = matrix_create("C", 3, 3);

		matrix_print(A);
		matrix_print(B);
		matrix_print(C);

		matrix_multiply(A, B, C);
		dynamic_programming_debug("\nresult:\n");
		matrix_print(C);

                // C (result)
		//   30 24 18
		//   84 69 54
		//   138 114 90

		CU_ASSERT(30 == C->m[0][0]);
		CU_ASSERT(24 == C->m[0][1]);
		CU_ASSERT(18 == C->m[0][2]);
		CU_ASSERT(84 == C->m[1][0]);
		CU_ASSERT(69 == C->m[1][1]);
		CU_ASSERT(54 == C->m[1][2]);
		CU_ASSERT(138 == C->m[2][0]);
		CU_ASSERT(114 == C->m[2][1]);
		CU_ASSERT(90 == C->m[2][2]);

		matrix_destroy(A);
		matrix_destroy(B);
		matrix_destroy(C);
	}
}

void test_matrix_chain_order(void)
{
	//  verifies by the book's example of six matrices
	//
	//  matrix    |   A1  |   A2  |   A3  |   A4  |   A5  |   A6
	//  ----------+-------+-------+-------+-------+-------+-------
	//  dimension | 30x35 | 35x15 | 15x5  |  5x10 | 10x20 | 20x25
	//  cols[i]   |    35 |    15 |    5  |    10 |    20 |    25
	//  rows[i]   | 30    | 35    | 15    |  5    | 10    | 20

	int cols[] = {35, 15, 5, 10, 20, 25};
	int rows[] = {30, 35, 15, 5, 10, 20};
	int size = 6; //sizeof(cols)/sizeof(*cols);
	memo_p memo = NULL;

	dynamic_programming_debug("\n");
	memo = matrix_chain_order(rows, cols, size);
	CU_ASSERT(NULL != memo);

	matrix_print(memo->mtable_min_costs);
	matrix_print(memo->mtable_solution_index);

	CU_ASSERT(0 == memo->mtable_min_costs->m[0][0]);
	CU_ASSERT(15750 == memo->mtable_min_costs->m[0][1]);
//	CU_ASSERT(7875 == memo->mtable_min_costs->m[0][2]);
//	CU_ASSERT(9375 == memo->mtable_min_costs->m[0][3]);
//	CU_ASSERT(11875 == memo->mtable_min_costs->m[0][4]);
//	CU_ASSERT(15125 == memo->mtable_min_costs->m[0][5]);

	CU_ASSERT(0 == memo->mtable_min_costs->m[1][1]);
	CU_ASSERT(2625 == memo->mtable_min_costs->m[1][2]);
	CU_ASSERT(4375 == memo->mtable_min_costs->m[1][3]);
	CU_ASSERT(7125 == memo->mtable_min_costs->m[1][4]);
	CU_ASSERT(10500 == memo->mtable_min_costs->m[1][5]);

	CU_ASSERT(0 == memo->mtable_min_costs->m[2][2]);
	CU_ASSERT(750 == memo->mtable_min_costs->m[2][3]);
	CU_ASSERT(2500 == memo->mtable_min_costs->m[2][4]);
	CU_ASSERT(5375 == memo->mtable_min_costs->m[2][5]);

	CU_ASSERT(0 == memo->mtable_min_costs->m[3][3]);
	CU_ASSERT(1000 == memo->mtable_min_costs->m[3][4]);
	CU_ASSERT(3500 == memo->mtable_min_costs->m[3][5]);

	CU_ASSERT(0 == memo->mtable_min_costs->m[4][4]);
	CU_ASSERT(5000 == memo->mtable_min_costs->m[4][5]);

	CU_ASSERT(0 == memo->mtable_min_costs->m[5][5]);

	matrix_destroy(memo->mtable_min_costs);
	matrix_destroy(memo->mtable_solution_index);
	free(memo);
}


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("dynamic-programming", init_suite_dynamic_programming,
			      clean_suite_dynamic_programming);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "matrix struct", test_matrix)
 		TEST_append(pSuite, "matrix-chain multiply",
		            test_matrix_multiply) // */
		TEST_append(pSuite, "matrix-chain order",
			    test_matrix_chain_order) // */
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
