/*
  dynamic programming: optimal binary search tree

  Suppose that we are designing a program to translate text from
  English to French. [...]. We want words that occur frequently in the
  text to be placed nearer the root. Moreover some words in the text
  might have no French translation, and such words would not appear in
  the binary search tree at all. How do we organize a binary search
  tree so as to minimize the number of nodes visited in all searches,
  given that we know how often each word occurs?

  What we need is known as an optimal binary search tree. Formally, we
  are given a sequence K = <k[1], k[2],...k[n]> of n distinct keys in
  sorted order (so that k[1] < k[2] < ... < k[n]), and we wish to
  build a binary search tree from these keys.

  For each key we have a probability p[i] that a search will be for
  k[i]. Some searches may be for values not in K, and so we also have
  n + 1 "dummy keys" d[0], d[1], d[2],... , d[n] representing values
  not in K. In particular d[0] represents all values less than k[1],
  d[n] represents all values greater than k[n], and for i = 1, 2, ...,
  n-1, the dummy key d[i] represents all values between k[i] k[i+1].

  For each dummy key d[i], we have a probability q[i] that a search
  will correspond to d[i], i.e.

  SUM(p[i]) + SUM(q[i]) == 1
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

void double_matrix_print(double_matrix_p mat)
{
	if (!mat) {
		return;
	}
	dynamic_programming_debug("\n%s =\n", mat->name);
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			dynamic_programming_debug("%f ", mat->m[idx][jdx]);
		}
		dynamic_programming_debug("\n");
	}
}


// dynamic-programming

/*
  OPTIMAL-BST(p, q, n)

  let e[1..n+1, 0..n],
      w[1..n+1, 0..n],
      root[1..n, 1..n] be new tables (matrices)

  for i = 1 to n+1
    e[i, i-1] = q[i-1]
    w[i, i-1] = q[i-1]
  for l = 1 to n
    for i = 1 to n-l+1
      j = i + l - 1
      e[i, j] = oo
      w[i, j] = w[i, j-1] + p[j] + q[j]
      for r = i to j
        t = e[i, r-1] + e[r+1, j] + w[i, j]
	if t < e[i, j]
	  e[i, j] = t
	  root[i, j] = r

  NB:
  p := probability of the particular key
  q := probability of corresponding dummy keys
  n := number of keys, w/o dummy
*/
optimal_bst_p optimal_bst(const double *p, const double *q, int len)
{
	optimal_bst_p opt;

	double_matrix_p e;
	double_matrix_p w;
	int_matrix_p root;

	opt = malloc(sizeof(*opt));
	if (!opt) {
		dynamic_programming_failure("allocation failed");
	}

	// all matrix indices start from 0, in case we just don't use
	// them, when starting from 1 instead, thus the '+2'
	e = double_matrix_create("E-table", len+2, len+1); /* [1..n+1][0..n] */
	w = double_matrix_create("w-table", len+2, len+1); /* [1..n+1][0..n] */
	root = int_matrix_create("root", len+1, len+2);        /* [1..n][1..n] */
	opt->e = e;
	opt->w = w;
	opt->root = root;

/*
	for (int idx = 1; idx < len+1; idx++) {
		e->m[idx][idx-1] = q[idx-1];
		w->m[idx][idx-1] = q[idx-1];
	}
	for (int ldx = 1; ldx < len; ldx++) {
		for (int idx = 1; idx < len - ldx + 1; idx++) {
			int jdx = idx + ldx - 1;  
			e->m[idx][jdx] = INT_MAX;
			w->m[idx][jdx] = w->m[idx][jdx-1]
				+ p[jdx]
				+ q[jdx];

			// perform possible computations
			for (int rdx = idx; rdx < jdx; rdx++) {
				double tval = e->m[idx][rdx-1]
					+ e->m[rdx+1][jdx]
					+ w->m[idx][jdx];

				// compare them, take the min(vals..)
				if (tval < e->m[idx][jdx]) {
					e->m[idx][jdx] = tval;
					root->m[idx][jdx] = rdx;
				}
			}
		}
	}
/*/
//	for (int idx = 1; idx < len+1; idx++) {
	for (int idx = 1; idx < len+2; idx++) {
		e->m[idx][idx-1] = q[idx-1];
		w->m[idx][idx-1] = q[idx-1];
	}
//	for (int ldx = 1; ldx < len; ldx++) {
	for (int ldx = 1; ldx < len; ldx++) {
//		for (int idx = 1; idx < len - ldx + 1; idx++) {
		for (int idx = 1; idx < len - ldx + 1; idx++) {
			int jdx = idx + ldx;
			e->m[idx][jdx] = INT_MAX;
			w->m[idx][jdx] = w->m[idx][jdx-1]
				+ p[jdx]
				+ q[jdx];

			// perform possible computations
			for (int rdx = idx; rdx < jdx; rdx++) {
				double tval = e->m[idx][rdx-1]
					+ e->m[rdx+1][jdx]
					+ w->m[idx][jdx];

				// compare them, take the min(vals..)
				if (tval < e->m[idx][jdx]) {
					e->m[idx][jdx] = tval;
					root->m[idx][jdx] = rdx;
				}
			}
		}
	}
// */

	// return result tables (matrices)
	return opt;
}
