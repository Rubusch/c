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
#include <limits.h>
#include "matrix.h"


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


  dynamic-programming approach

  problem setup and variables in the above algorithm description
  NB: we denote matrices as M[nrows, ncols]

  matrix | nrows | ncols
  -------+-------+-------
     A  :=    p  x  q
     B  :=    q  x  r
   => C :=    p  x  r

  -> matrix multiplication A x B = C, requires A.q (A->ncols) and B.q
     (B->ncols) to be equal!

  thus, we define
  q := A.ncols = B.nrows
  p := A.nrows
  r := B.ncols

  NB: A and B are ambiguously chosen names, to explain what is q, p
  and r int each matrix multiplication.


  example

  multiplication of six matrices (n = 6)

  matrix    |   A1  |   A2  |   A3  |   A4  |   A5  |   A6
  ----------+-------+-------+-------+-------+-------+-------
  dimension | 30x35 | 35x15 | 15x5  |  5x10 | 10x20 | 20x25
  p[i]      |    35 |    15 |    5  |    10 |    20 |    25
  r[i]      | 30    | 35    | 15    |  5    | 10    | 20

  e.g. idx = 1; jdx = 2; m[1,2] = r[1] * p[1] * p[2] // A1 -> idx = 0, not 1!!

  NB: redefinition of some above variables:
      p := A.ncols (former/above 'q')
      q := min cost !!!


  arguments

      r := auxiliary rows array to init the auxiliary mtable_min_costs
           accordingly

      p := cols array, to compute the initial values of the auxiliary
           mtable_min_costs

      len := length of array p or r, the number of matrices to chain


  NB: this does no matrix multiplication at all, it computes what is
  the minimal cost possible for best chaining approach (multiplying
  and setting parethesis); it does not even provide the particular
  chaining which has this minimal cost!
*/
memo_p matrix_chain_order(int *r, int *p, int len)
{
	matrix_p mtable_min_costs;
	mtable_min_costs = matrix_create("MIN_COSTS", len, len);
	matrix_p mtable_solution_index;
	mtable_solution_index = matrix_create("SOLUTION_INDEX", len, len);

	// pre-init of auxiliary matrix mtable_min_costs
	matrix_init_all(mtable_min_costs, INT_MAX);
	for (int idx = 0; idx < len; idx++) {
		for (int jdx = 0; jdx < len; jdx++) {
			if (jdx == idx) {
				mtable_min_costs->m[idx][idx] = 0;
				continue;
			}
			if (jdx < idx) {
				// just visualization
				mtable_min_costs->m[idx][jdx] = -1;
			}
		}
	}

	for (int chain_len = 1; chain_len < len; chain_len++) {
		// chain_len is the chain length
		for (int idx = 0; idx < len - chain_len; idx++) {
			int jdx = idx + chain_len;
			for (int kdx = idx; kdx < jdx; kdx++) {

// debugging the algorthm //////////////////////////////////////////////////////
dynamic_programming_debug(
	"min_cost = m[%d,%d] + m[%d,%d] + r[%d]p[%d]p[%d] = %d + %d + %d * %d * %d = %d\n",
	idx, kdx,  (kdx+1), jdx,  idx, kdx, jdx,
	mtable_min_costs->m[idx][kdx], mtable_min_costs->m[kdx+1][jdx],
	r[idx], p[kdx], p[jdx],
	(mtable_min_costs->m[idx][kdx] + mtable_min_costs->m[kdx+1][jdx]
	 + r[idx] * p[kdx] * p[jdx])
);
////////////////////////////////////////////////////////////////////////////////

				int min_cost = mtable_min_costs->m[idx][kdx]
					+ mtable_min_costs->m[kdx+1][jdx]
					+ r[idx] * p[kdx] * p[jdx];

				// now, find the min_cost among all costs
				if (min_cost < mtable_min_costs->m[idx][jdx]) {

// debugging the algorthm //////////////////////////////////////////////////////
dynamic_programming_debug(
	"mtable_min_costs->m[%d,%d] = min_cost = %d\n",
	idx, jdx, min_cost
);
////////////////////////////////////////////////////////////////////////////////

                                        mtable_min_costs->m[idx][jdx] = min_cost;
					mtable_solution_index->m[idx][jdx] = kdx;
				}
			}
		}
	}

	memo_p memo = malloc(sizeof(*memo));
	if (!memo) {
		dynamic_programming_failure("allocation failed");
	}
	memo->mtable_min_costs = mtable_min_costs;
	memo->mtable_solution_index = mtable_solution_index;

	// NB: needs external
	// matrix_destroy(memo->mtable_min_costs),
	// matrix_destroy(memo->mtable_solution_index), and
	// free(memo)

	return memo;
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

    a recursive printer for the dynamic-programming approach
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

