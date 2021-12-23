/*
  dynamic programming: the rod cutting problem

  Given a rod of length n inches and a table of prices p[i] for i = 1,
  2,..., n, determine the maximum revenue r[n] obtainable by cutting up
  the rod and selling the pieces. Note that if the price p[n] for a
  rod of length n is large enough, an optimal solution may require no
  cutting at all.

  length i   | 1 | 2 | 3 | 4 |  5 |  6 |  7 |  8 |  9 | 10 |
  -----------+---+---+---+---+----+----+----+----+----+----+
  price p[i] | 1 | 5 | 8 | 9 | 10 | 17 | 17 | 20 | 24 | 30 |


  NB: The dynamic programming rod cutting solutions solve the rod
  cutting problem - it DOES NOT NEED TO TELL HOW TO CUT the rod. It is
  asked, though, for the max. revenue, which is possible.
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

/*
  CUT-ROD(p, n)
  the inefficient approach, O(2^n)

  if n == 0
    return 0
  q = -oo
  for i = 1 to n
    q = max(q, p[i] + CUT-ROD(p, n-i))
  return q


  ...where...
  p[1..n] is an array of prices
  n is a length of a rod / rod cut
  q is the maximum revenue

  this shows +/- the following recursion:
      for 1->10
       for 1->9
        for 1->8
         for 1->7
	  (...)
	 (...)
      for 1->9
       (...)
      for 1->8
       (...)
      for 1->7
      (...)

      -> O(n^n)

  NB: number of prizes == len of rod or shifted by 1, since prices[0]
  is the first element for the first "cut"!

  NB: debug output and numbers are NOT THE ROD FRAGMENTS to cut!!! it
  is rather a small hint where we stand inside the recursion
*/
int cut_rod(const int *prices, int len)
{
	if (len == 0) {
		return 0;
	}

	int debug_idx = -1;
	int revenue = 0;
	for (int idx = 1; idx < len; idx++) {
		int revenue_sub = cut_rod(prices, len - idx);
		int revenue_max = max(revenue, (prices[idx] + revenue_sub));

		// debug printing
		if (revenue != revenue_max) debug_idx = idx;

		revenue = revenue_max;
	}

	// debug printing
	if (debug_idx > 0) {
		dynamic_programming_debug("revenue: %d, from solution %d\n",
					  revenue, debug_idx);
	}

	return revenue;
}

/*
  MEMOIZED-CUT-ROD-AUX(p, n, r)

  if r[n] >= 0
    return r[n]
  if n == 0
    q = 0
  else
    q = -oo
    for i = 1 to n
      q = max(q, p[i] + MEMOIZED-CUT-ROD-AUX(p, n-i, r))
  r[n] = q
  return q

  r := the memo

  NB: A dynamic-programming approach runs in polynomial time when the
  number of distinct subproblems involved is polynomial in the input
  size and we can solve each such subproblem in polynomial time
 */
int _memoized_cut_rod(const int *prices, int len, int* memo)
{
	if (len == 0) {
		return 0;
	}

	int revenue = 0;
	int debug_idx = -1;
	if (memo[len-1] >= 0) {
		return memo[len-1]; //  return in case memo has something
	}

	if (len-1 == 0) {
		revenue = prices[0];
	} else {
		revenue = -1;
		for (int idx = 1; idx < len; idx++) {
			int revenue_sub = _memoized_cut_rod(prices, len - idx,
								memo);
			int revenue_max = max(revenue, (prices[idx]+revenue_sub));

			// debug printing
			if (revenue != revenue_max) debug_idx = idx;

			revenue = revenue_max;
		}
	}
	memo[len-1] = revenue; // registered revenue, in case is 0,
			       // does not need to be checked again as
			       // in NAIVE approach

	// debug printing
	if (debug_idx > 0) {
		for (int idx = 0; idx < len; idx++) {
			if (memo[idx] > 0) {
				dynamic_programming_debug(" %d", idx);
			}
		}
		dynamic_programming_debug(" revenue: %d, from solution %d\n",
					  revenue, debug_idx);
	}

	return revenue;
}

/*
  MEMOIZED-CUT-ROD(p, n)

  let r[0..n] be a new array
  for i = 0 to n
    r[i] = -oo
  return MEMOIZED-CUT-ROD-AUX(p, n, r)
*/
int memoized_cut_rod(const int *prices, int len)
{
	int *memo = malloc((len) * sizeof(*memo));
	if (!memo) {
		dynamic_programming_failure("allocation failed");
	}

	memset(memo, -1, len * sizeof(*memo));
	int revenue = _memoized_cut_rod(prices, len, memo);

	free(memo);

	return revenue;
}

/*
  BOTTOM-UP-CUT-ROD(p, n)

  let r[0..n] be a new array
  r[0] = 0
  for j = 1 to n
    q = -oo
    for i = q to j
      q = max(q, p[i]+r[j-i])
    r[j] = q
  return r[n]

  NB: kind of an iterative approach to dynamic-programming
  implementations
 */
int bottom_up_cut_rod(const int *prices, int len)
{
	int *memo = malloc(len * sizeof(*memo));
	if (!memo) {
		dynamic_programming_failure("allocation failed");
	}
	memset(memo, -1, (len-1) * sizeof(*memo));

	memo[0] = 0;
	for (int jdx = 1; jdx < len; jdx++) {
		int revenue = 0;
		for (int idx = revenue; idx < jdx; idx++) {
			revenue = max(revenue, prices[idx] + memo[jdx - idx]);
		}
		memo[jdx] = revenue;
	}
	int result = memo[len-1];

	// debug printing
	for (int idx = 0; idx < len; idx++) {
		dynamic_programming_debug("%d: %d\n", idx, memo[idx]);
	}
	dynamic_programming_debug("revenue: %d\n", result);

	free(memo);
	return result;
}

/*
  EXTENDED-BOTTOM-UP-CUT-ROD(p, n)

  let r[0..n] and s[0..n] be new arrays
  r[0] = 0
  for j = 1 to n
    q = -oo
    for i = 1 to j
      if q < p[i] + r[j-i]
        q = p[i] + r[j-i]
	s[j] = i
    r[j] = q
  return r and s


  NB: The dynamic-programming solutions to the rod-cutting problem
  return the value of an optimal solution, but they do not return an
  acutal solution: a list of piece sizes.

  This is an extended version of BOTTOM-UP-CUT-ROD that computes, for
  each rod size j, not only the maximum revenue r[j], but also s[j],
  the optimal size of the first piece to cut off.

  memo.r := the revenue
  memo.s := the first rod size to cut
 */
memo_p _extended_bottom_up_cut_rod(const int *prices, int len, memo_p memo)
{
	int revenue = -1;

	for (int jdx = 1; jdx < len; jdx++) {
		revenue = -1;
		for (int idx = 1; idx <= jdx; idx++) {
			if (revenue < prices[idx] + memo->r[jdx - idx]) {
				revenue = prices[idx] + memo->r[jdx - idx];
				memo->s[jdx] = idx;
			}
		}
		memo->r[jdx] = revenue;
	}

	return memo;
}


/*
  PRINT-CUT-ROD-SOLUTION(p, n)

  (r, s) = EXTENDED-BOTTOM-UP-CUT-ROD(p, n)
  while n > 0
    print s[n]
    n = n - s[n]
 */
int print_cut_rod_solution(const int *prices, int len)
{
	// startup
	memo_p memo;
	memo = malloc(sizeof(*memo));
	if (!memo) {
		dynamic_programming_failure("allocation failed");
	}

	memo->r = malloc(len * sizeof(*memo->r));
	if (!memo->r) {
		dynamic_programming_failure("allocation failed");
	}
	memset(memo->r, 0, len);

	memo->s = malloc(len * sizeof(*memo->s));
	if (!memo->s) {
		dynamic_programming_failure("allocation failed");
	}
	memset(memo->s, 0, len);

	// actual rod cutting
	memo = _extended_bottom_up_cut_rod(prices, len, memo);
	int res = memo->r[len-1];

	dynamic_programming_debug("cut sizes: ");
	while (len > 1) {
		dynamic_programming_debug("%d ", memo->s[len-1]);
		len -= memo->s[len-1];
	}
	dynamic_programming_debug("\n");

	// teardown
	if (memo->r) {
		free(memo->r);
		memo->r = NULL;
	}
	if (memo->s) {
		free(memo->s);
		memo->s = NULL;
	}
	free(memo);

	return res;
}
