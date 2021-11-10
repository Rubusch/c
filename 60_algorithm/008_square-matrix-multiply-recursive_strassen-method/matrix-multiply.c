/*
  square matrix multiplication

  A, B are n x n matrices
  A x B = C
 */

#include "matrix-multiply.h"

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

void free_square_matrix(const int NROWS, int ***matrix)
{
	int **M = *matrix;
	int idx;
	for (idx = 0; idx < NROWS; idx++) {
		free(M[idx]); M[idx] = NULL;
	}
	free(*matrix);
	matrix = NULL;
}

void matrix_add(matrix_t *A, matrix_t *B, matrix_t *C)
{
	const int NCOLS = A->ncols;
	const int NROWS = B->nrows;
	int idx, jdx;

	for (idx = 0; idx < NROWS; idx++) {
		for (jdx = 0; jdx < NCOLS; jdx++) {
			C->val[idx][jdx] = A->val[idx][jdx] + B->val[idx][jdx];
		}
	}
	C->ncols = NCOLS;
	C->nrows = NROWS;
}

void matrix_negate(matrix_t *A, matrix_t *C)
{
	const int NCOLS = A->ncols;
	const int NROWS = A->nrows;
	int idx, jdx;

	for (idx = 0; idx < NROWS; idx++) {
		for (jdx = 0; jdx < NCOLS; jdx++) {
			C->val[idx][jdx] = -(A->val[idx][jdx]);
		}
	}
	C->ncols = NCOLS;
	C->nrows = NROWS;
}

// allocates memory for matrix elements!!!
void matrix_divide(matrix_t *M, matrix_t* A, matrix_t* B, matrix_t* C, matrix_t* D)
{
	const int NROWS = M->nrows;
	const int NROWS2 = NROWS/2;
	const int NCOLS = M->ncols;
	const int NCOLS2 = NCOLS/2;
	int idx, jdx;

	alloc_square_matrix(NROWS2, &A->val);
	for (idx = 0; idx < NROWS2; idx++) {
		for (jdx = 0; jdx < NCOLS2; jdx++) {
			A->val[idx][jdx] = M->val[idx][jdx];
		}
	}
	A->nrows = NROWS2;
	A->ncols = NCOLS2;

	alloc_square_matrix(NROWS2, &B->val);
	for (idx = 0; idx < NROWS2; idx++) {
		for (jdx = NCOLS2; jdx < NCOLS; jdx++) {
			B->val[idx][jdx - NCOLS2] = M->val[idx][jdx];
		}
	}
	B->nrows = NROWS2;
	B->ncols = NCOLS - NCOLS2;

	alloc_square_matrix(NROWS - NROWS2, &C->val);
	for (idx = NROWS2; idx < NROWS; idx++) {
		for (jdx = 0; jdx < NCOLS2; jdx++) {
			C->val[idx - NROWS2][jdx] = M->val[idx][jdx];
		}
	}
	C->nrows = NROWS - NROWS2;
	C->ncols = NCOLS2;

	alloc_square_matrix(NROWS - NROWS2, &D->val);
	for (idx = NROWS2; idx < NROWS; idx++) {
		for (jdx = NCOLS2; jdx < NCOLS; jdx++) {
			D->val[idx - NROWS2][jdx - NCOLS2] = M->val[idx][jdx];
		}
	}
	D->nrows = NROWS - NROWS2;
	D->ncols = NCOLS - NCOLS2;
}

// allocates new matrix and elments, frees passed matrices and their elements
matrix_t* matrix_combine(matrix_t* A, matrix_t* B, matrix_t* C, matrix_t* D)
{
	matrix_t* M;
	int idx, jdx;

	if (NULL == (M = malloc(sizeof(*M)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	M->nrows = A->nrows + C->nrows;
	M->ncols = A->ncols + B->ncols;
	alloc_square_matrix(M->nrows, &M->val);

	for (idx = 0; idx < M->nrows; idx++) { // more efficiently would be to re-link the allocated memory
		for (jdx = 0; jdx < M->ncols; jdx++) {
			if (idx < A->nrows && jdx < A->ncols) {
				M->val[idx][jdx] = A->val[idx][jdx];
			} else if (idx < A->nrows && jdx >= A->ncols) {
				M->val[idx][jdx] = B->val[idx][jdx - A->ncols];
			} else if (idx >= A->nrows && jdx < A->ncols) {
				M->val[idx][jdx] = C->val[idx - A->nrows][jdx];
			} else if (idx >= A->nrows && jdx >= A->ncols) {
				M->val[idx][jdx] = D->val[idx - A->nrows][jdx - A->ncols];
			} else {
				fprintf(stderr, "ERROR: something weird happened\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	free_square_matrix(A->nrows, &A->val);
	free(A);

	free_square_matrix(B->nrows, &B->val);
	free(B);

	free_square_matrix(C->nrows, &C->val);
	free(C);

	free_square_matrix(D->nrows, &D->val);
	free(D);

	return M;
}


matrix_t* strassen(matrix_t *A, matrix_t* B)
{
	const int NCOLS = A->ncols;
	const int NROWS = B->nrows;

	if (NROWS == 1 && NCOLS == 1) {
		matrix_t* C;
		if (NULL == (C = malloc(sizeof(*C)))) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}
		C->nrows = NCOLS;
		C->ncols = NROWS;
		alloc_square_matrix(C->nrows, &C->val);

		C->val[0][0] = A->val[0][0] * B->val[0][0];

		return C;
	}

	matrix_t *M0, *M1, *M2, *M3, *M4, *M5, *M6;
	matrix_t *a00, *a01, *a10, *a11;
	matrix_t *b00, *b01, *b10, *b11;
	matrix_t *c00, *c01, *c10, *c11;
	matrix_t *tmp_a, *tmp_b, *tmp_c;

	if (NULL == (tmp_a = malloc(sizeof(*tmp_a)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (tmp_b = malloc(sizeof(*tmp_b)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (tmp_c = malloc(sizeof(*tmp_c)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (a00 = malloc(sizeof(*a00)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (a01 = malloc(sizeof(*a01)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (a10 = malloc(sizeof(*a10)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (a11 = malloc(sizeof(*a11)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (b00 = malloc(sizeof(*b00)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (b01 = malloc(sizeof(*b01)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (b10 = malloc(sizeof(*b10)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (b11 = malloc(sizeof(*b11)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (c00 = malloc(sizeof(*c00)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (c01 = malloc(sizeof(*c01)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (c10 = malloc(sizeof(*c10)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	if (NULL == (c11 = malloc(sizeof(*c11)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}


	// DIVIDE: split matrices A and B into quadrants each

	matrix_divide(A, a00, a01, a10, a11);
	matrix_divide(B, b00, b01, b10, b11);


	// CONQUER: recursive strassen() calls

        /* M0 = strassen(a00, b01 - b11) */
	tmp_a->nrows = b11->nrows;
	tmp_a->ncols = b11->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_negate(b11, tmp_a);

	tmp_b->nrows = b01->nrows;
	tmp_b->ncols = b01->ncols;
	alloc_square_matrix(tmp_b->nrows, &tmp_b->val);

	matrix_add(b01, tmp_a, tmp_b);

	M0 = strassen(a00, tmp_b);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_b->nrows, &tmp_b->val);


	/* M1 = strassen(a00 + a01, b11) */
	tmp_a->nrows = a00->nrows;
	tmp_a->ncols = a00->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_add(a00, a01, tmp_a);

	M1 = strassen(tmp_a, b11);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);


	/* M2 = strassen(a10 + a11, b00) */
	tmp_a->nrows = a10->nrows;
	tmp_a->ncols = a10->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_add(a10, a11, tmp_a);

	M2 = strassen(tmp_a, b00);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);


	/* M3 = strassen(a11, b10 - b00) */
	tmp_a->nrows = b00->nrows;
	tmp_a->ncols = b00->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_negate(b00, tmp_a);

	tmp_b->nrows = b10->nrows;
	tmp_b->ncols = b10->ncols;
	alloc_square_matrix(tmp_b->nrows, &tmp_b->val);

	matrix_add(b10, tmp_a, tmp_b);

	M3 = strassen(a11, tmp_b);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_b->nrows, &tmp_b->val);


	/* M4 = strassen(a00 + a11, b00 + b11) */
	tmp_a->nrows = a00->nrows;
	tmp_a->ncols = a00->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_add(a00, a11, tmp_a);

	tmp_b->nrows = b00->nrows;
	tmp_b->ncols = b00->ncols;
	alloc_square_matrix(tmp_b->nrows, &tmp_b->val);

	matrix_add(b00, b11, tmp_b);

	M4 = strassen(tmp_a, tmp_b);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_b->nrows, &tmp_b->val);


	/* M5 = strassen(a01 - a11, b10 + b11) */
	tmp_a->nrows = a11->nrows;
	tmp_a->ncols = a11->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_negate(a11, tmp_a);

	tmp_b->nrows = a01->nrows;
	tmp_b->ncols = a01->ncols;
	alloc_square_matrix(tmp_b->nrows, &tmp_b->val);

	matrix_add(a01, tmp_a, tmp_b);

	tmp_c->nrows = b10->nrows;
	tmp_c->ncols = b10->ncols;
	alloc_square_matrix(tmp_c->nrows, &tmp_c->val);

	matrix_add(b10, b11, tmp_c);

	M5 = strassen(tmp_b, tmp_c);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_b->nrows, &tmp_b->val);
	free_square_matrix(tmp_c->nrows, &tmp_c->val);


	/* M6 = strassen(a00 - a10, b00 + b01) */
	tmp_a->nrows = a10->nrows;
	tmp_a->ncols = a10->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_negate(a10, tmp_a);

	tmp_b->nrows = a00->nrows;
	tmp_b->ncols = a00->ncols;
	alloc_square_matrix(tmp_b->nrows, &tmp_b->val);

	matrix_add(a00, tmp_a, tmp_b);

	tmp_c->nrows = b00->nrows;
	tmp_c->ncols = b00->ncols;
	alloc_square_matrix(tmp_c->nrows, &tmp_c->val);

	matrix_add(b00, b01, tmp_c);

	M6 = strassen(tmp_b, tmp_c);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_b->nrows, &tmp_b->val);
	free_square_matrix(tmp_c->nrows, &tmp_c->val);


	// free
	free_square_matrix(a00->nrows, &a00->val);
	free(a00);
	free_square_matrix(a01->nrows, &a01->val);
	free(a01);
	free_square_matrix(a10->nrows, &a10->val);
	free(a10);
	free_square_matrix(a11->nrows, &a11->val);
	free(a11);

	free_square_matrix(b00->nrows, &b00->val);
	free(b00);
	free_square_matrix(b01->nrows, &b01->val);
	free(b01);
	free_square_matrix(b10->nrows, &b10->val);
	free(b10);
	free_square_matrix(b11->nrows, &b11->val);
	free(b11);


	// setting up quadrant matrices

	/* c00 = M4 + M3 - M1 + M5 */
	tmp_a->nrows = M4->nrows;
	tmp_a->ncols = M4->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_add(M4, M3, tmp_a);

	tmp_b->nrows = M1->nrows;
	tmp_b->ncols = M1->ncols;
	alloc_square_matrix(tmp_b->nrows, &tmp_b->val);

	matrix_negate(M1, tmp_b);

	tmp_c->nrows = M1->nrows;
	tmp_c->ncols = M1->ncols;
	alloc_square_matrix(tmp_c->nrows, &tmp_c->val);

	matrix_add(tmp_a, tmp_b, tmp_c);

	c00->nrows = M5->nrows;
	c00->ncols = M5->ncols;
	alloc_square_matrix(c00->nrows, &c00->val);

	matrix_add(tmp_c, M5, c00); // c00

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_b->nrows, &tmp_b->val);
	free_square_matrix(tmp_c->nrows, &tmp_c->val);


	/* c01 = M0 + M1 */
	c01->nrows = M0->nrows;
	c01->ncols = M0->ncols;
	alloc_square_matrix(c01->nrows, &c01->val);

	matrix_add(M0, M1, c01); // c01


	/* c10 = M2 + M3 */
	c10->nrows = M2->nrows;
	c10->ncols = M2->ncols;
	alloc_square_matrix(c10->nrows, &c10->val);

	matrix_add(M2, M3, c10); // c10


	/* c11 = M0 + M4 - M2 - M6 */
	tmp_a->nrows = M0->nrows;
	tmp_a->ncols = M0->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_add(M0, M4, tmp_a);

	tmp_b->nrows = M2->nrows;
	tmp_b->ncols = M2->ncols;
	alloc_square_matrix(tmp_b->nrows, &tmp_b->val);

	matrix_negate(M2, tmp_b);

	tmp_c->nrows = M2->nrows;
	tmp_c->ncols = M2->ncols;
	alloc_square_matrix(tmp_c->nrows, &tmp_c->val);

	matrix_add(tmp_a, tmp_b, tmp_c);

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_b->nrows, &tmp_b->val);

	tmp_a->nrows = M6->nrows;
	tmp_a->ncols = M6->ncols;
	alloc_square_matrix(tmp_a->nrows, &tmp_a->val);

	matrix_negate(M6, tmp_a);

	c11->nrows = M6->nrows;
	c11->ncols = M6->ncols;
	alloc_square_matrix(c11->nrows, &c11->val);

	matrix_add(tmp_c, tmp_a, c11); // c11

	free_square_matrix(tmp_a->nrows, &tmp_a->val);
	free_square_matrix(tmp_c->nrows, &tmp_c->val);


	// free
	free(tmp_a);
	free(tmp_b);
	free(tmp_c);

	free_square_matrix(M0->nrows, &M0->val);
	free(M0);
	free_square_matrix(M1->nrows, &M1->val);
	free(M1);
	free_square_matrix(M2->nrows, &M2->val);
	free(M2);
	free_square_matrix(M3->nrows, &M3->val);
	free(M3);
	free_square_matrix(M4->nrows, &M4->val);
	free(M4);
	free_square_matrix(M5->nrows, &M5->val);
	free(M5);
	free_square_matrix(M6->nrows, &M6->val);
	free(M6);

	return matrix_combine(c00, c01, c10, c11); // frees c00, c01, c10 and c11
}


void square_matrix_multiply(const int NROWS, int **A,
			    int **B, int **C)
{
/*      // simple multiplication
	int idx, jdx, kdx;

	for (idx = 0; idx < NROWS; idx++) {
		for (jdx = 0; jdx < NROWS; jdx++) {
			int tmp = 0;
			for (kdx = 0; kdx < NROWS; kdx++) {
			       tmp = tmp + A[idx][kdx] * B[kdx][jdx];
			}
			C[idx][jdx] = tmp;
		}
	}
/*/
	matrix_t *mat_A, *mat_B, *mat_C;
	int idx, jdx;

	if (NULL == (mat_A = malloc(sizeof(*mat_A)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	mat_A->nrows = NROWS;
	mat_A->ncols = NROWS;
	alloc_square_matrix(mat_A->nrows, &mat_A->val);
	for (idx = 0; idx < mat_A->nrows; idx++) {
		for (jdx = 0; jdx < mat_A->ncols; jdx++) {
			mat_A->val[idx][jdx] = A[idx][jdx];
		}
	}

	if (NULL == (mat_B = malloc(sizeof(*mat_B)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	mat_B->nrows = NROWS;
	mat_B->ncols = NROWS;
	alloc_square_matrix(mat_B->nrows, &mat_B->val);
	for (idx = 0; idx < mat_B->nrows; idx++) {
		for (jdx = 0; jdx < mat_B->ncols; jdx++) {
			mat_B->val[idx][jdx] = B[idx][jdx];
		}
	}

	mat_C = strassen(mat_A, mat_B);

	for (idx = 0; idx < mat_C->nrows; idx++) {
		for (jdx = 0; jdx < mat_C->ncols; jdx++) {
			C[idx][jdx] = mat_C->val[idx][jdx];
		}
	}

	free_square_matrix(mat_A->nrows, &mat_A->val);
	free(mat_A);
	free_square_matrix(mat_B->nrows, &mat_B->val);
	free(mat_B);
	free_square_matrix(mat_C->nrows, &mat_C->val);
	free(mat_C);
}

