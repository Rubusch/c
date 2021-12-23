/*
  dynamic programming: matrix-chain multiplication

  Given a sequence (chain) <A[1], A[2], ...., A[n]> of n matrices to
  be multiplied, and we wish to compute the product
  A[1]*A[2]*...*A[n].

  We can evaluate the expression using the standard algorithm for
  multiplying pairs of matrices as a subroutine once we have
  parenthesized it to solve all ambiguities in how the matrices are
  multiplied together. Matrix multiplication is associative and so all
  parenthesizations yield the same product.

  How we parethesize a chain of matrices can have a dramatic impact on
  the (performance) cost of evaluating the product.



  dynamic-programming approach

  1. Characterize the structure of an optimal solution
  2. Recursively define the value of an optimal solution
  3. Compute the value of an optimal solution
  4. Construct an optimal solution from computed information


  NB: In the matrix-chain multiplication problem, we are not actually
  multiplying matrices. Our goal is only to determine an order for
  multiplying matrices that has the lowest (performance) cost.
  Typically, the time invested in determining this optimal order
  is more than paid for by the time saved later on when actually
  performing the matrix multiplications, such as performing only 7500
  scalar multiplications instead of 75,000.
 */

#include "dynamic-programming.h"

#include <stdarg.h>


/* utils */

void dynamic_programming_debug(const char* format, ...)
{
#ifdef DEBUG
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
#endif
}

void dynamic_programming_failure(const char* format, ...)
{
	perror("failed!");
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
	exit(EXIT_FAILURE);
}

// matrix
matrix_p matrix_create(const char *name, int ncols, int nrows)
{
	matrix_p mat = malloc(sizeof(*mat));
	if (!mat) {
		dynamic_programming_failure("allocation failed");
	}

	mat->m = malloc(nrows * sizeof(*mat->m));
	for (int idx = 0; idx < nrows; idx++) {
		mat->m[idx] = malloc(ncols * sizeof(*mat->m));
		for (int jdx = 0; jdx < ncols; jdx++) {
			mat->m[idx][jdx] = 0;
		}
	}

	if (strlen(name) >= MATRIX_NAME_SIZE) {
		dynamic_programming_failure("matrix name overrun");
	}
	memset(mat->name, '\0', (MATRIX_NAME_SIZE-1) * sizeof(*mat->name));
	strncpy(mat->name, name, strlen(name));
	mat->name[MATRIX_NAME_SIZE-1] = '\0';
	mat->ncols = ncols;
	mat->nrows = nrows;

	return mat;
}

void matrix_destroy(matrix_p mat)
{
	if (!mat) {
		return;
	}

	for (int idx = 0; idx < mat->nrows; idx++) {
		free(mat->m[idx]);
	}
	free(mat->m);
	free(mat);
}

void matrix_init_row(matrix_p mat, int rowidx, int* vals, int vals_size)
{
	if (!mat) return;
	if (rowidx >= mat->nrows) return;
	if (vals_size != mat->ncols) return;

	for (int idx = 0; idx < mat->ncols; idx++) {
		mat->m[rowidx][idx] = vals[idx];
	}
}

void matrix_print(matrix_p mat)
{
#ifdef DEBUG
	if (!mat) {
		return;
	}
	dynamic_programming_debug("\n%s =\n", mat->name);
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			dynamic_programming_debug("%d ", mat->m[idx][jdx]);
		}
		dynamic_programming_debug("\n");
	}
#endif /* DEBUG */
}


// dynamic-programming

/*
  MATRIX-MULTIPLY(A, B)

  if A.columns != B.rows
    error "incompatible dimensions"
  else
    let C be a new A.rows x B.columns matrix
    for i = 1 to A.rows
      for j = 1 to B.columns
        c[i,j] = 0
	for k = 1 to A.columns
	  c[i,j] = c[i,j] + a[i,k] * b[k,j]
    return C
*/
int matrix_multiply(const matrix_p A, const matrix_p B , matrix_p C)
{
	if (!A) return -1;
	if (!B) return -2;
	if (!C) return -3;

	if (A->ncols != B->nrows) {
		dynamic_programming_failure("%s(): incompatible dimensions\n",
					    __func__);
	}
	if (C->nrows != A->nrows) return -4;
	if (C->ncols != B->ncols) return -5;

	for (int idx = 0; idx < A->nrows; idx++) {
		for (int jdx = 0; jdx < B->ncols; jdx++) {
			C->m[idx][jdx] = 0;
			for (int kdx = 0; kdx < A->ncols; kdx++) {
				C->m[idx][jdx] = C->m[idx][jdx]
					+ A->m[idx][kdx] * B->m[kdx][jdx];
			}
		}
	}
	return 0;
}

/*
  MATRIX-CHAIN-ORDER(p)

  n = p.length - 1
  let m[1..n,1..n] and s[1..n-1, 2..n] be new tables
  for i = 1 to n
    m[i,i] = 0
  for l = 2 to n
    for i = 1 to n - l + 1
      j = i + l - 1
      m[i,j] = oo
      for k = i to j - 1
        q = m[i,k] + m[k + 1,j] + p[i-1]p[k]p[j]
	if q < m[i,j]
	  m[i,j] = q
	  s[i,j] = k
  return m and s
*/
memo_p matrix_chain_order(matrix_p p)
{
	
	return NULL;
}

/*
  PRINT-OPTIMAL-PARENS(s, i, j)

  if i == j
    print "A"[i]
  else
    print "("
    PRINT-OPTIMAL-PARENS(s, i, s[i,j])
    PRINT-OPTIMAL-PARENS(s, s[i,j] + 1, j)
    print ")"
*/
void print_optimal_parens(int s, int i, int j)
{
	
}

/*
  RECURSIE-MATRIX-CHAIN(p, i, k)

  if i == j
    return 0
  m[i,j] = oo
  for k = i to j-1
    q = RECURSIVE-MATRIX-CHAIN(p, i, k)
        + RECURSIVE-MATRIX-CHAIN(p, k + 1, j)
	+ p[i-1]p[k]p[j]
    if q < m[i,j]
      m[i,j] = q
  return m[i,j]
*/
// TODO       

/*
  LOOKUP-CHAIN(m, p, i, j)

  if m[i, j] < oo
    return m[i, j]
  if i == j
    m[i, j] = 0
  else
    for k = i to j - 1
      q = LOOKUP-CHAIN(m, p, i, k)
          + LOOKUP-CHAIN(m, p, k + 1, j)
	  + p[i-1]p[k]p[j]
      if q < m[i, j]
        m[i, j] = q
  return m[i, j]
*/
// TODO       

/*
  MEMOIZED-MATRIX-CHAIN(p)

  n = p.length - 1
  let m[1..n,1..n] be a new table
  for i = 1 to n
    for j = i to n
      m[i,j] = oo
  return LOOKUP-CHAIN(m, p, 1, n)
*/
// TODO       

