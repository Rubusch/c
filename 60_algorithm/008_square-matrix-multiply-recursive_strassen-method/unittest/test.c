/*
  cunit - tests

  2D matrices implementations

  - static 2D arrays:
     arr[NROWS][NROWS], then the function definition would be
     func(int arr[NROWS][NROWS]);
    have to be fixed in both dimensions, only the last dimension could
    be left open

  - VLAs:
     arr[NROWs][NROWS], then a function call with VLA would be
     func(const int NROWS, int arr[NROWS][NROWS]);

  - allocated memory:
     int **arr;
     arr = malloc(NROws * sizeof(**arr));
     for ....NROWs; idx++) {
        arr[idx] = malloc(NROWS * sizeof(*arr));
     }
    and the function:
     func(int **arr);

    allocated memory is the only one which scales
 */
#include "test.h"
#include "stdlib.h"

void test_matrix_add(void)
{
	matrix_t *A, *B, *C;
	const int NROWS = 2;

	if (NULL == (A = malloc(sizeof(*A)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	alloc_square_matrix(NROWS, &A->val);
	A->nrows = NROWS;
	A->ncols = NROWS;

	if (NULL == (B = malloc(sizeof(*B)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	alloc_square_matrix(NROWS, &B->val);
	B->nrows = NROWS;
	B->ncols = NROWS;

	if (NULL == (C = malloc(sizeof(*C)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	alloc_square_matrix(NROWS, &C->val);


	A->val[0][0] = 1;
	A->val[0][1] = 2;
	A->val[1][0] = 3;
	A->val[1][1] = 4;

	B->val[0][0] = 1;
	B->val[0][1] = 2;
	B->val[1][0] = 3;
	B->val[1][1] = 4;

	C->val[0][0] = 0;
	C->val[0][1] = 0;
	C->val[1][0] = 0;
	C->val[1][1] = 0;

	// testee
	matrix_add(A, B, C);

	CU_ASSERT(2 == C->val[0][0]);
	CU_ASSERT(4 == C->val[0][1]);
	CU_ASSERT(6 == C->val[1][0]);
	CU_ASSERT(8 == C->val[1][1]);

	free_square_matrix(A->nrows, &(A->val));
	free(A);

	free_square_matrix(B->nrows, &(B->val));
	free(B);

	free_square_matrix(C->nrows, &(C->val));
	free(C);
}

void test_matrix_negate(void)
{
	matrix_t *A, *C;
	const int NROWS = 2;

	if (NULL == (A = malloc(sizeof(*A)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	alloc_square_matrix(NROWS, &A->val);
	A->nrows = NROWS;
	A->ncols = NROWS;

	if (NULL == (C = malloc(sizeof(*C)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	alloc_square_matrix(NROWS, &C->val);


	A->val[0][0] = 1;
	A->val[0][1] = 2;
	A->val[1][0] = 3;
	A->val[1][1] = 4;

	C->val[0][0] = 0;
	C->val[0][1] = 0;
	C->val[1][0] = 0;
	C->val[1][1] = 0;

	// testee
	matrix_negate(A, C);

	CU_ASSERT(-1 == C->val[0][0]);
	CU_ASSERT(-2 == C->val[0][1]);
	CU_ASSERT(-3 == C->val[1][0]);
	CU_ASSERT(-4 == C->val[1][1]);

	free_square_matrix(A->nrows, &(A->val));
	free(A);

	free_square_matrix(C->nrows, &(C->val));
	free(C);
}

void test_matrix_divide(void)
{
	matrix_t *M, *A, *B, *C, *D;
	const int NROWS = 4;
	const int NCOLS = NROWS;

	if (NULL == (M = malloc(sizeof(*M)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	alloc_square_matrix(NROWS, &M->val);
	M->nrows = NROWS;
	M->ncols = NCOLS;

	if (NULL == (A = malloc(sizeof(*A)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (B = malloc(sizeof(*B)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (C = malloc(sizeof(*C)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (D = malloc(sizeof(*D)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	M->val[0][0] = 1;
	M->val[0][1] = 1;
	M->val[1][0] = 1;
	M->val[1][1] = 1;
	M->val[0][2] = 2;
	M->val[0][3] = 2;
	M->val[1][2] = 2;
	M->val[1][3] = 2;
	M->val[2][0] = 3;
	M->val[2][1] = 3;
	M->val[3][0] = 3;
	M->val[3][1] = 3;
	M->val[2][2] = 4;
	M->val[2][3] = 4;
	M->val[3][2] = 4;
	M->val[3][3] = 4;

	// testee
	matrix_divide(M, A, B, C, D);

	CU_ASSERT(1 == A->val[0][0]);
	CU_ASSERT(1 == A->val[0][1]);
	CU_ASSERT(1 == A->val[1][0]);
	CU_ASSERT(1 == A->val[1][1]);

	CU_ASSERT(2 == B->val[0][0]);
	CU_ASSERT(2 == B->val[0][1]);
	CU_ASSERT(2 == B->val[1][0]);
	CU_ASSERT(2 == B->val[1][1]);

	CU_ASSERT(3 == C->val[0][0]);
	CU_ASSERT(3 == C->val[0][1]);
	CU_ASSERT(3 == C->val[1][0]);
	CU_ASSERT(3 == C->val[1][1]);

	CU_ASSERT(4 == D->val[0][0]);
	CU_ASSERT(4 == D->val[0][1]);
	CU_ASSERT(4 == D->val[1][0]);
	CU_ASSERT(4 == D->val[1][1]);

	free_square_matrix(M->nrows, &M->val);
	free(M);

	free_square_matrix(A->nrows, &A->val);
	free(A);

	free_square_matrix(B->nrows, &B->val);
	free(B);

	free_square_matrix(C->nrows, &C->val);
	free(C);

	free_square_matrix(D->nrows, &D->val);
	free(D);
}

void test_matrix_combine(void)
{
	matrix_t *M, *A, *B, *C, *D;
	const int NROWS = 2;
	const int NCOLS = NROWS;

	if (NULL == (A = malloc(sizeof(*A)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	A->nrows = NROWS;
	A->ncols = NCOLS;
	alloc_square_matrix(NROWS, &A->val);
	A->val[0][0] = 1;
	A->val[0][1] = 1;
	A->val[1][0] = 1;
	A->val[1][1] = 1;

	if (NULL == (B = malloc(sizeof(*B)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	B->nrows = NROWS;
	B->ncols = NCOLS;
	alloc_square_matrix(NROWS, &B->val);
	B->val[0][0] = 2;
	B->val[0][1] = 2;
	B->val[1][0] = 2;
	B->val[1][1] = 2;

	if (NULL == (C = malloc(sizeof(*C)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	C->nrows = NROWS;
	C->ncols = NCOLS;
	alloc_square_matrix(NROWS, &C->val);
	C->val[0][0] = 3;
	C->val[0][1] = 3;
	C->val[1][0] = 3;
	C->val[1][1] = 3;

	if (NULL == (D = malloc(sizeof(*D)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	D->nrows = NROWS;
	D->ncols = NCOLS;
	alloc_square_matrix(NROWS, &D->val);
	D->val[0][0] = 4;
	D->val[0][1] = 4;
	D->val[1][0] = 4;
	D->val[1][1] = 4;

	// testee
	M = matrix_combine(A, B, C, D);

	CU_ASSERT(1 == M->val[0][0]);
	CU_ASSERT(1 == M->val[0][1]);
	CU_ASSERT(1 == M->val[1][0]);
	CU_ASSERT(1 == M->val[1][1]);

	CU_ASSERT(2 == M->val[0][2]);
	CU_ASSERT(2 == M->val[0][3]);
	CU_ASSERT(2 == M->val[1][2]);
	CU_ASSERT(2 == M->val[1][3]);

	CU_ASSERT(3 == M->val[2][0]);
	CU_ASSERT(3 == M->val[2][1]);
	CU_ASSERT(3 == M->val[3][0]);
	CU_ASSERT(3 == M->val[3][1]);

	CU_ASSERT(4 == M->val[2][2]);
	CU_ASSERT(4 == M->val[2][3]);
	CU_ASSERT(4 == M->val[3][2]);
	CU_ASSERT(4 == M->val[3][3]);

	free_square_matrix(M->nrows, &M->val);
	free(M);
}

/*
     |1 2|   |5 7|   |11  9|
     |3 4| x |3 1| = |27 25|
*/
void test_2x2_multiply(void)
{
	const int ARR_SIZE = 2;
	int **A, **B, **C;

	alloc_square_matrix(ARR_SIZE, &A);
	alloc_square_matrix(ARR_SIZE, &B);
	alloc_square_matrix(ARR_SIZE, &C);

	A[0][0] = 1;
	A[0][1] = 2;
	A[1][0] = 3;
	A[1][1] = 4;

	B[0][0] = 5;
	B[0][1] = 7;
	B[1][0] = 3;
	B[1][1] = 1;

	square_matrix_multiply(ARR_SIZE, A, B, C);

	print_matrix(ARR_SIZE, "C (result)", C);

	CU_ASSERT(11 == C[0][0]);
	CU_ASSERT(9 == C[0][1]);
	CU_ASSERT(27 == C[1][0]);
	CU_ASSERT(25 == C[1][1]);

	free_square_matrix(ARR_SIZE, &A);
	free_square_matrix(ARR_SIZE, &B);
	free_square_matrix(ARR_SIZE, &C);
}

int main()
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("utilities", init_suite_success,
			      clean_suite_success);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	TEST_start(pSuite, "matrix add", test_matrix_add)
		TEST_append(pSuite, "matrix negate", test_matrix_negate)
		TEST_append(pSuite, "matrix divide", test_matrix_divide)
		TEST_append(pSuite, "matrix combine", test_matrix_combine)
	TEST_end();


	pSuite = CU_add_suite("strassen", init_suite_strassen,
			      clean_suite_strassen);
	TEST_start(pSuite, "strassen matrix multiplication", test_2x2_multiply)
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

	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
