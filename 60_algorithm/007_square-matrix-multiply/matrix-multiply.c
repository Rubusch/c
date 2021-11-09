/*
  square matrix multiplication

  A, B are n x n matrices
  A x B = C
 */

#include "matrix-multiply.h"

void square_matrix_multiply(const int NROWS, int **A,
			    int **B, int **C)
{
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
}
