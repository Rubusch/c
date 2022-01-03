/*
  cunit - tests
 */
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "dynamic-programming.h"
#include "matrix.h"
#include "test.h"


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

//	double pvals[] = {0.15, 0.10, 0.05, 0.10, 0.20};
	double pvals[] = {0.0, 0.15, 0.10, 0.05, 0.10, 0.20};
	int pvals_len = sizeof(pvals)/sizeof(*pvals);

	double qvals[] = {0.05, 0.10, 0.05, 0.05, 0.05, 0.10};
//	int qvals_len = sizeof(qvals)/sizeof(*qvals);

	int len = pvals_len;
	optimal_bst_p opti_bst;
	opti_bst = optimal_bst(pvals, qvals, len);

	double_matrix_print(opti_bst->e);
	CU_ASSERT(double_relatively_equal(0.05, opti_bst->e->m[1][0]));
	CU_ASSERT(double_relatively_equal(0.45, opti_bst->e->m[1][1]));
	CU_ASSERT(double_relatively_equal(0.90, opti_bst->e->m[1][2]));
	CU_ASSERT(double_relatively_equal(1.25, opti_bst->e->m[1][3]));
	CU_ASSERT(double_relatively_equal(1.75, opti_bst->e->m[1][4]));
	CU_ASSERT(double_relatively_equal(2.75, opti_bst->e->m[1][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[2][0]));
	CU_ASSERT(double_relatively_equal(0.10, opti_bst->e->m[2][1]));
	CU_ASSERT(double_relatively_equal(0.40, opti_bst->e->m[2][2]));
	CU_ASSERT(double_relatively_equal(0.70, opti_bst->e->m[2][3]));
	CU_ASSERT(double_relatively_equal(1.20, opti_bst->e->m[2][4]));
	CU_ASSERT(double_relatively_equal(2.00, opti_bst->e->m[2][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[3][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[3][1]));
	CU_ASSERT(double_relatively_equal(0.05, opti_bst->e->m[3][2]));
	CU_ASSERT(double_relatively_equal(0.25, opti_bst->e->m[3][3]));
	CU_ASSERT(double_relatively_equal(0.60, opti_bst->e->m[3][4]));
	CU_ASSERT(double_relatively_equal(1.30, opti_bst->e->m[3][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[4][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[4][1]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[4][2]));
	CU_ASSERT(double_relatively_equal(0.05, opti_bst->e->m[4][3]));
	CU_ASSERT(double_relatively_equal(0.30, opti_bst->e->m[4][4]));
	CU_ASSERT(double_relatively_equal(0.90, opti_bst->e->m[4][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[5][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[5][1]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[5][2]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[5][3]));
	CU_ASSERT(double_relatively_equal(0.05, opti_bst->e->m[5][4]));
	CU_ASSERT(double_relatively_equal(0.50, opti_bst->e->m[5][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[6][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[6][1]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[6][2]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[6][3]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->e->m[6][4]));
	CU_ASSERT(double_relatively_equal(0.10, opti_bst->e->m[6][5]));

// TODO	          

	double_matrix_print(opti_bst->w);

	CU_ASSERT(double_relatively_equal(0.05, opti_bst->w->m[1][0]));
	CU_ASSERT(double_relatively_equal(0.30, opti_bst->w->m[1][1]));
	CU_ASSERT(double_relatively_equal(0.45, opti_bst->w->m[1][2]));
	CU_ASSERT(double_relatively_equal(0.55, opti_bst->w->m[1][3]));
	CU_ASSERT(double_relatively_equal(0.70, opti_bst->w->m[1][4]));
	CU_ASSERT(double_relatively_equal(1.00, opti_bst->w->m[1][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[2][0]));
	CU_ASSERT(double_relatively_equal(0.10, opti_bst->w->m[2][1]));
	CU_ASSERT(double_relatively_equal(0.25, opti_bst->w->m[2][2]));
	CU_ASSERT(double_relatively_equal(0.35, opti_bst->w->m[2][3]));
	CU_ASSERT(double_relatively_equal(0.50, opti_bst->w->m[2][4]));
	CU_ASSERT(double_relatively_equal(0.80, opti_bst->w->m[2][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[3][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[3][1]));
	CU_ASSERT(double_relatively_equal(0.05, opti_bst->w->m[3][2]));
	CU_ASSERT(double_relatively_equal(0.15, opti_bst->w->m[3][3]));
	CU_ASSERT(double_relatively_equal(0.30, opti_bst->w->m[3][4]));
	CU_ASSERT(double_relatively_equal(0.60, opti_bst->w->m[3][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[4][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[4][1]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[4][2]));
	CU_ASSERT(double_relatively_equal(0.05, opti_bst->w->m[4][3]));
	CU_ASSERT(double_relatively_equal(0.20, opti_bst->w->m[4][4]));
	CU_ASSERT(double_relatively_equal(0.50, opti_bst->w->m[4][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[5][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[5][1]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[5][2]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[5][3]));
	CU_ASSERT(double_relatively_equal(0.05, opti_bst->w->m[5][4]));
	CU_ASSERT(double_relatively_equal(0.35, opti_bst->w->m[5][5]));

	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[6][0]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[6][1]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[6][2]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[6][3]));
	CU_ASSERT(double_relatively_equal(0.00, opti_bst->w->m[6][4]));
	CU_ASSERT(double_relatively_equal(0.10, opti_bst->w->m[6][5]));

	int_matrix_print(opti_bst->root);

	CU_ASSERT(1 == opti_bst->root->m[1][1]);    
	CU_ASSERT(1 == opti_bst->root->m[1][2]);
	CU_ASSERT(2 == opti_bst->root->m[1][3]);
	CU_ASSERT(2 == opti_bst->root->m[1][4]);
	CU_ASSERT(2 == opti_bst->root->m[1][5]);

	CU_ASSERT(0 == opti_bst->root->m[2][1]);
	CU_ASSERT(2 == opti_bst->root->m[2][2]);
	CU_ASSERT(2 == opti_bst->root->m[2][3]);
	CU_ASSERT(2 == opti_bst->root->m[2][4]);
	CU_ASSERT(4 == opti_bst->root->m[2][5]);

	CU_ASSERT(0 == opti_bst->root->m[3][1]);
	CU_ASSERT(0 == opti_bst->root->m[3][2]);
	CU_ASSERT(3 == opti_bst->root->m[3][3]);
	CU_ASSERT(4 == opti_bst->root->m[3][4]);
	CU_ASSERT(5 == opti_bst->root->m[3][5]);

	CU_ASSERT(0 == opti_bst->root->m[4][1]);
	CU_ASSERT(0 == opti_bst->root->m[4][2]);
	CU_ASSERT(0 == opti_bst->root->m[4][3]);
	CU_ASSERT(4 == opti_bst->root->m[4][4]);
	CU_ASSERT(5 == opti_bst->root->m[4][5]);

	CU_ASSERT(0 == opti_bst->root->m[5][1]);
	CU_ASSERT(0 == opti_bst->root->m[5][2]);
	CU_ASSERT(0 == opti_bst->root->m[5][3]);
	CU_ASSERT(0 == opti_bst->root->m[5][4]);
	CU_ASSERT(5 == opti_bst->root->m[5][5]);

// TODO 	

	// delete
// FIXME
//	double_matrix_destroy(opti_bst->e);
//	double_matrix_destroy(opti_bst->w);
//	int_matrix_destroy(opti_bst->root);
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
