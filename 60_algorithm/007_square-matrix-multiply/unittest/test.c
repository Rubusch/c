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

//#define DEBUG 1

void print_matrix(const int NROWS, const char* name, int **matrix)
{
#ifdef DEBUG
	int idx, jdx;

	fprintf(stderr, "\n%s = \n", name);
	for (idx = 0; idx < NROWS; idx++) {
		for (jdx = 0; jdx < NROWS; jdx++) {
			fprintf(stderr, " %d", matrix[idx][jdx]);
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
#endif
}

void alloc_square_matrix(const int NROWS, int ***matrix)
{
	int **M;
	int idx;

	M = malloc(NROWS * sizeof(*M));
	if (!M) {
		perror("allocation failed, first dimension");
		exit(EXIT_FAILURE);
	}
	for (idx = 0; idx < NROWS; idx++) {
		M[idx] = malloc(NROWS * sizeof(**M));
		if (!M[idx]) {
			perror("allocation failed, second dimension");
			exit(EXIT_FAILURE);
		}
	}

	*matrix = M;
}

void free_square_matrix( const int NROWS, int ***matrix)
{
	int **M = *matrix;
	int idx;
	for (idx = 0; idx < NROWS; idx++) {
		free(M[idx]); M[idx] = NULL;
	}
	free(*matrix);
	matrix = NULL;
}

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

void test_2x2_simple_multiply(void)
{
	const int ARR_SIZE = 2;
	int **A, **B, **C;

	alloc_square_matrix(ARR_SIZE, &A);
	alloc_square_matrix(ARR_SIZE, &B);
	alloc_square_matrix(ARR_SIZE, &C);

	int idx, jdx;
	for (idx = 0; idx < ARR_SIZE; idx++) {
		for (jdx = 0; jdx < ARR_SIZE; jdx++) {
			A[idx][jdx] = jdx;
		}
	}
	print_matrix(ARR_SIZE, "A", A);

	for (idx = 0; idx < ARR_SIZE; idx++) {
		for (jdx = 0; jdx < ARR_SIZE; jdx++) {
			B[idx][jdx] = idx;
		}
	}
	print_matrix(ARR_SIZE, "B", B);

	for (idx = 0; idx < ARR_SIZE; idx++) {
		for (jdx = 0; jdx < ARR_SIZE; jdx++) {
			C[idx][jdx] = 0;
		}
	}
	print_matrix(ARR_SIZE, "C", C);

	square_matrix_multiply(ARR_SIZE, A, B, C);

	print_matrix(ARR_SIZE, "C (result)", C);

	CU_ASSERT(1 == C[0][0]);
	CU_ASSERT(1 == C[0][1]);
	CU_ASSERT(1 == C[1][0]);
	CU_ASSERT(1 == C[1][1]);

	free_square_matrix(ARR_SIZE, &A);
	free_square_matrix(ARR_SIZE, &B);
	free_square_matrix(ARR_SIZE, &C);
}

void test_random_multiply(void)
{
	const int ARR_SIZE = 50;
	int **A, **B, **C;
	int upper = ARR_SIZE;
	int lower = 0;
	int idx, jdx, tmp;

	alloc_square_matrix(ARR_SIZE, &A);
	alloc_square_matrix(ARR_SIZE, &B);
	alloc_square_matrix(ARR_SIZE, &C);
	srand(time(0));
	for (idx = 0; idx < ARR_SIZE; idx++) {
		for (jdx = 0; jdx < ARR_SIZE; jdx++) {
			A[idx][jdx] = (rand() % (upper - lower + 1)) + lower;
			B[idx][jdx] = (rand() % (upper - lower + 1)) + lower;
			C[idx][jdx] = 0;
		}
	}

	print_matrix(ARR_SIZE, "C", C);
	square_matrix_multiply(ARR_SIZE, A, B, C);
	print_matrix(ARR_SIZE, "C (result)", C);

	CU_ASSERT(0 <= C[0][0]);

	tmp = 0;
	for (idx = 0; idx < ARR_SIZE; idx++) {
		tmp += A[0][idx] * B[idx][0];
	}
	CU_ASSERT(C[0][0] == tmp);

	tmp = 0;
	for (idx = 0; idx < ARR_SIZE; idx++) {
		tmp += A[1][idx] * B[idx][0];
	}
	CU_ASSERT(C[1][0] == tmp);

	tmp = 0;
	for (idx = 0; idx < ARR_SIZE; idx++) {
		tmp += A[2][idx] * B[idx][0];
	}
	CU_ASSERT(C[2][0] == tmp);

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
	pSuite = CU_add_suite("Suite_success", init_suite_success,
			      clean_suite_success);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	TEST_start(pSuite, "2x2 multiply", test_2x2_multiply)
		TEST_append(pSuite, "simple 2x2 multiply", test_2x2_simple_multiply)
		TEST_append(pSuite, "random matrix multiply", test_random_multiply)
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
