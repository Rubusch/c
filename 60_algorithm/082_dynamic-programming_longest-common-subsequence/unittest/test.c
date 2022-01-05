/*
  cunit - tests
 */

#include "dynamic-programming.h"
#include "matrix.h"
#include "queue.h"

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
		int_matrix_destroy(&mat);
	}
	{
		char_matrix_p mat = NULL;
		CU_ASSERT(NULL == mat);
		mat = char_matrix_create("CHAR", 10, 10);
		char_matrix_init_all(mat, '0');
		CU_ASSERT(NULL != mat);
		char_matrix_print(mat);

		CU_ASSERT('0' == mat->m[ 7][0]);
		CU_ASSERT('0' == mat->m[ 7][1]);
		CU_ASSERT('0' == mat->m[ 7][2]);
		CU_ASSERT('0' == mat->m[ 7][3]);
		CU_ASSERT('0' == mat->m[ 7][4]);
		CU_ASSERT('0' == mat->m[ 7][5]);
		CU_ASSERT('0' == mat->m[ 7][6]);
		CU_ASSERT('0' == mat->m[ 7][7]);
		CU_ASSERT('0' == mat->m[ 7][8]);
		CU_ASSERT('0' == mat->m[ 7][9]);

		char arr[] = {'A', 'B', 'C', 'B', 'D', 'A', 'B', 'A', 'B', 'C'};
		int arr_size = sizeof(arr) / sizeof(*arr);
		char_matrix_init_row(mat, 7, arr, arr_size);
		char_matrix_print(mat);

		CU_ASSERT('A' == mat->m[ 7][0]);
		CU_ASSERT('B' == mat->m[ 7][1]);
		CU_ASSERT('C' == mat->m[ 7][2]);
		CU_ASSERT('B' == mat->m[ 7][3]);
		CU_ASSERT('D' == mat->m[ 7][4]);
		CU_ASSERT('A' == mat->m[ 7][5]);
		CU_ASSERT('B' == mat->m[ 7][6]);
		CU_ASSERT('A' == mat->m[ 7][7]);
		CU_ASSERT('B' == mat->m[ 7][8]);
		CU_ASSERT('C' == mat->m[ 7][9]);

		// delete
		char_matrix_destroy(&mat);
	}

}

void test_lcs_length(void)
{
	{
		// NB: must start with {'0',... prefix
		char x[] = {'0', 'A', 'B', 'C', 'B', 'D', 'A', 'B'};
		int xlen = sizeof(x) / sizeof(*x);

		// NB: must start with {'0',... prefix
		char y[] = {'0', 'B', 'D', 'C', 'A', 'B', 'A'};
		int ylen = sizeof(y) / sizeof(*y);

		char res[8]; memset(res, '\0', xlen);

		lcs_p lcs;
		dynamic_programming_debug("\n");
		lcs = lcs_length(x, xlen, y, ylen);
		int_matrix_print(lcs->c);
		char_matrix_print(lcs->b);

		lcs_print(lcs->b, x, xlen, xlen-1, ylen-1);
		int idx = 0;
		while (!fifo__empty()) {
			char ch;
			fifo__dequeue(&ch);
			res[idx] = ch;
			idx++;
		}
		res[idx+1] = '\0';

		dynamic_programming_debug("solution: %s [BCBA]\n", res);

		CU_ASSERT('B' == res[0]);
		CU_ASSERT('C' == res[1]);
		CU_ASSERT('B' == res[2]);
		CU_ASSERT('A' == res[3]);

		// delete
		int_matrix_destroy(&lcs->c);
		char_matrix_destroy(&lcs->b);
		free(lcs);
	}
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
 		TEST_append(pSuite, "lcs length", test_lcs_length)
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
