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

 */

#include "dynamic-programming.h"

#include <stdarg.h>

/* utils */

void debug(const char* format, ...)
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

  NB: nPrizes == len of rod! we may cut the rod at each inch/meter,
  the prices gives a table for every possible resulting size (i.e. len
  is discrete)
 */
int cut_rod(const int *prices, int len)
{
	if (len == 0) {
		debug(" 0\t- ");
		return 0;
	} else {
		debug(" %d", len);
	}

	int revenue = 0;
	for (int idx = 0; idx < len; idx++) {
		int revenue_sub = cut_rod(prices, len - (idx+1));
		revenue = max(revenue, (prices[idx] + revenue_sub));
	}
	debug(" last cut at %d, revenue: %d\n", len, revenue);

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
 */
int _memoized_cut_rod_aux(const int *prices, int len, int* arr)
{
	if (len == 0) {
		debug(" 0\t- ");
		return 0;
	} else {
		debug(" %d", len);
	}

	int revenue = 0;
	if (arr[len-1] >= 0) {
		return arr[len-1];
	}

	if (len-1 == 0) {
		revenue = prices[0];
	} else {
		revenue = -1;
		for (int idx = 0; idx < len; idx++) {
			int revenue_sub = _memoized_cut_rod_aux(prices, (len - (idx+1)), arr);
			revenue = max(revenue, (prices[idx] + revenue_sub));
		}
	}
	arr[len-1] = revenue;
	debug(" last cut at %d, revenue: %d\n", len, revenue);

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
	int *arr = malloc((len) * sizeof(*arr));
	if (!arr) {
		dynamic_programming_failure("allocation failed");
	}

	memset(arr, -1, len * sizeof(*arr));
	int revenue = _memoized_cut_rod_aux(prices, len, arr);

	free(arr);

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
 */
int bottom_up_cut_rod(const int *prices, int len)
{
	
	int *arr = malloc(len * sizeof(*arr));
	if (!arr) {
		dynamic_programming_failure("allocation failed");
	}

	arr[0] = 0;
	for (int jdx = 1; jdx < len; jdx++) {
		int revenue = -1;
		for (int idx = revenue; idx < jdx; idx++) {
			revenue = max(revenue, prices[idx] + arr[jdx - idx]);
		}
		arr[jdx] = revenue;
	}

	return arr[len-1];
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
 */
memo_p extended_bottom_up_cut_rod(const int *prices, int len, memo_p memo)
{
	int revenue = -1;

	for (int jdx = 0; jdx < len; jdx++) {
		revenue = -1;
		for (int idx = 0; idx <= jdx; idx++) {
			if (revenue < prices[idx] + memo->r[jdx - idx]) {
				revenue = prices[idx] + memo->r[jdx - idx];
				memo->s[jdx] = idx;
			}
		}
		memo->r[jdx] = revenue; // TODO check level of indention    
	}

	return memo; // TODO figure out results of this   
}


/*
  PRINT-CUT-ROD-SOLUTION(p, n)

  (r, s) = EXTENDED-BOTTOM-UP-CUT-ROD(p, n)
  while n > 0
    print s[n]
    n = n - s[n]
 */
void print_cut_rod_solution(const int *prices, int len)
{
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


	memo = extended_bottom_up_cut_rod(prices, len, memo);
	while (len > 0) {
		debug("%d ", memo->s[len]);
		len = len - memo->s[len];
	}
	debug("\n");


	if (memo->r) {
		free(memo->r);
		memo->r = NULL;
	}
	if (memo->s) {
		free(memo->s);
		memo->s = NULL;
	}
	free(memo);
}
