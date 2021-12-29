/*
  dynamic programming: longest common subsequence

  Biological applications often need to compare the DNA of two (or
  more) different organisms. A strand of DNA consists of a string of
  molecules called bases (A, C, G, T).

  Neither a sequence S1 nor another one S2 is a substring of the
  other. Alternatively we could say that two strands are similar if
  the number of changes needed to turn one into the other is
  small. Yet another way to measure the similarity of strands S1 and
  S2 is by finding a third strand S3 in which the bases in S3 appear
  in each of S1 and S2 is by finding a third strand S3 in which the
  bases in S3 appear in each of S1 and S2; these bases must appear in
  the same order, but not necessarily consecutively. The longer the
  strand S3 we can find, the more similar S1 and S2 are.

  We formalize this last notion of similarity as the
  longest-common-subsequence problem. A subsequence of a given
  sequence is just the given sequence with zero or more elements left
  out.


  dynamic-programming approach

  In the longest-common-subsequence problem, we are given two
  sequences X = <x1, x2, x3,..., xm> and Y = <y1, y2, y3,..., yn> and
  wish to find a maximum-length common subsequence of X and Y.

  1. Characterize the structure of an optimal solution
  2. Recursively define the value of an optimal solution
  3. Compute the value of an optimal solution
  4. Construct an optimal solution from computed information
 */

#include "dynamic-programming.h"
#include "matrix.h"

#include <limits.h>

// some additional headers needed for matrix_create()
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

// redundant scaffolding codes...
void int_matrix_print(int_matrix_p mat)
{
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
}

void char_matrix_print(char_matrix_p mat)
{
	if (!mat) {
		return;
	}
	dynamic_programming_debug("\n%s =\n", mat->name);
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			dynamic_programming_debug("%c ", mat->m[idx][jdx]);
		}
		dynamic_programming_debug("\n");
	}
}


// dynamic-programming

/*
  LCS-LENGTH(X, Y)

  m = X.length
  n = Y.length
  let b[1..m, 1..n] and c[0..m, 0..n] be new tables
  for i = 1 to m
    c[i,0] = 0
  for j = 0 to n
    c[0,j] = 0
  for i = 1 to m
    for j = 1 to n
      if x[i] == y[j]
        c[i,j] = c[i-1,j-1] + 1
	b[i,j] = "\"
      elseif c[i-1,j] >= c[i,j-1]
        c[i,j] = c[i-1,j]
	b[i,j] = "|"
      else
        c[i,j] = c[i,j-1]
	b[i,j] = "-"
  return c and b

  further improvements
  - we can eliminate the b table altogether
  - we can reduce the asymptotic space requirements for LCS-LENGTH
    sice it needs only two rows of table c at a time
*/
lcs_p lcs_length(const char *x, int xlen, const char *y, int ylen)
{
	// m == xlen
	// n == ylen
	lcs_p lcs;
	char_matrix_p bmat;
	int_matrix_p cmat;
	bmat = char_matrix_create("B", xlen, ylen);
	cmat = int_matrix_create("C", xlen, ylen);
	lcs = malloc(sizeof(*lcs));
	if (!lcs) {
		dynamic_programming_failure("allocation failed");
	}
	lcs->b = bmat;
	lcs->c = cmat;

	for (int idx = 1; idx < xlen; idx++) {
		cmat->m[idx][0] = 0;
	}
	for (int jdx = 0; jdx < ylen; jdx++) {
		cmat->m[0][jdx] = 0;
	}
	for (int idx = 1; idx < xlen; idx++) {
		for (int jdx = 1; jdx < ylen; jdx++) {
			if (x[idx] == y[jdx]) {
				cmat->m[idx][jdx] = cmat->m[idx-1][jdx-1] + 1;
				bmat->m[idx][jdx] = '\\';
			} else if (cmat->m[idx-1][jdx] >= cmat->m[idx][jdx-1]) {
				cmat->m[idx][jdx] = cmat->m[idx-1][jdx];
				bmat->m[idx][jdx] = '|';
			} else {
				cmat->m[idx][jdx] = cmat->m[idx][jdx-1];
				bmat->m[idx][jdx] = '-';
			}
		}
	}

	return lcs;
}

/*
  PRINT-LCS(b, X, i, j)

  if i == 0 or j == 0
    return
  if b[i,j] == "\"
    PRINT-LCS(b, X, i-1, j-1)
    print x[i]
  elseif b[i,j] == "|"
    PRINT-LCS(b, X, i-1, j)
  else
    PRINT-LCS(b, X, i, j-1)
*/
void lcs_print(char_matrix_p b, const char *x, int xlen, int idx, int jdx)
{
	if (idx == 0 || jdx == 0) {
		return;
	}
	if (b->m[idx][jdx] == '\\') {
		lcs_print(b, x, xlen, idx-1, jdx-1);
		dynamic_programming_debug("%c", x[idx]);
	} else if (b->m[idx][jdx] == '|') {
		lcs_print(b, x, xlen, idx-1, jdx);
	} else {
		lcs_print(b, x, xlen, idx, jdx-1);
	}
}

