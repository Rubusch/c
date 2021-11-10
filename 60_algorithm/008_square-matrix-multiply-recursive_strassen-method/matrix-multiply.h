/*
  divide & conquer approach

  (algorithm implementation, i.e. performative in terms of memory management)

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */
#ifndef MATRIX_MULTIPLY
#define MATRIX_MULTIPLY


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct matrix_s {
	int **val;
	int nrows;
	int ncols;
} matrix_t;


//#define DEBUG 1

void print_matrix(const int NROWS, const char* name, int **matrix);
void alloc_square_matrix(const int NROWS, int ***matrix);
void free_square_matrix(const int NROWS, int ***matrix);

void square_matrix_multiply(const int NROWS, int **A,
			    int **B, int **C);

void matrix_add(matrix_t *A, matrix_t *B, matrix_t *C);
void matrix_negate(matrix_t *A, matrix_t *C);
void matrix_divide(matrix_t *M, matrix_t* A, matrix_t* B, matrix_t* C, matrix_t* D);
matrix_t* matrix_combine(matrix_t* A, matrix_t* B, matrix_t* C, matrix_t* D);
matrix_t* strassen(matrix_t *A, matrix_t* B);


#endif
