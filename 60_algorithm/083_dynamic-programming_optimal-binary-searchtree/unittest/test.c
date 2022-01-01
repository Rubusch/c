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
	{
		int_matrix_p mat = NULL;
		CU_ASSERT(NULL == mat);
		mat = int_matrix_create("INT", 10, 10);
		CU_ASSERT(NULL != mat);
		int_matrix_print(mat);
		int arr[] = {1,2,3,4,5,6,7,8,9,10};
		int arr_size = sizeof(arr) / sizeof(*arr);
		int_matrix_init_row(mat, 7, arr, arr_size);
		int_matrix_print(mat);

		// delete
		int_matrix_destroy(mat);
		mat = NULL;
	}

	{
		double_matrix_p mat = NULL;
		CU_ASSERT(NULL == mat);
		mat = double_matrix_create("DOUBLE", 10, 10);

		CU_ASSERT(NULL != mat);
		double_matrix_print(mat);
		double arr[] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 1.0};
		int arr_size = sizeof(arr) / sizeof(*arr);
		double_matrix_init_row(mat, 7, arr, arr_size);
		double_matrix_print(mat);

		// delete
		double_matrix_destroy(mat);
		mat = NULL;
	}
}

/*
  test setup for optimal-bst (book example for verification)

  i    | 0    | 1    | 2    | 3    | 4    | 5
  -----+------+------+------+------+------+-------
  p[i] |      | 0.15 | 0.10 | 0.05 | 0.10 | 0.20
  q[i] | 0.05 | 0.10 | 0.05 | 0.05 | 0.05 | 0.10

*/
void test_optimal_binary_search_tree(void)
{

	double pvals[] = {0.15, 0.10, 0.05, 0.10, 0.20};
	int pvals_len = sizeof(pvals)/sizeof(*pvals);

	double qvals[] = {0.05, 0.10, 0.05, 0.05, 0.05, 0.10};
//	int qvals_len = sizeof(qvals)/sizeof(*qvals);

	int len = pvals_len;
	optimal_bst_p opti_bst;
	opti_bst = optimal_bst(pvals, qvals, len);

	double_matrix_print(opti_bst->e);
	double_matrix_print(opti_bst->w);
	int_matrix_print(opti_bst->root);

	// delete
// FIXME
	double_matrix_destroy(opti_bst->e);
	double_matrix_destroy(opti_bst->w);
	int_matrix_destroy(opti_bst->root);
	free(opti_bst);
}


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("dynamic-programming",
			      init_suite_dynamic_programming,
			      clean_suite_dynamic_programming);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "matrix struct", test_matrix)
 		TEST_append(pSuite, "optimal binary search tree",
			    test_optimal_binary_search_tree) // */
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
