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


/* utils */

#ifdef DEBUG

#ifndef LINE_SIZE
#define LINE_SIZE 64
#endif

static char line[LINE_SIZE];
static FILE* fp;

#endif /* DEBUG */


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
 */
int cut_rod(const int *prices, const int nprices, int length)
{
	if (length == 0) {
		return 0;
	}

	int revenue = 0;
	for (int idx = 0; idx < length; idx++) {
		fprintf(stderr, " %d", length-(idx-1));    
		int revenue_sub = cut_rod(prices, nprices, length - (idx+1) );
		revenue = max(revenue, (prices[idx] + revenue_sub));
	}
	fprintf(stderr, " - %d\n", revenue);    

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
 */
int _memoized_cut_rod_aux(const int *prices, const int nprices, int length, int* arr)
{
	int revenue = 0;
	if (arr[length] >= 0) {
		return arr[length];
	}
	if (length == 0) {
		revenue = 0;
	} else {
		revenue = -1;
	}
	for (int idx = 1; idx < length; idx++) {
		int revenue_sub = prices[idx]
			+ _memoized_cut_rod_aux(prices, nprices, (length - idx), arr);
		revenue = max(revenue, revenue_sub);
	}
	arr[length] = revenue;

	return revenue;
}

/*
  MEMOIZED-CUT-ROD(p, n)

  let r[0..n] be a new array
  for i = 0 to n
    r[i] = -oo
  return MEMOIZED-CUT-ROD-AUX(p, n, r)
*/
int memoized_cut_rod(const int *prices, const int nprices, int length)
{
	int *arr = malloc(length * sizeof(*arr));
	if (!arr) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	for (int idx = 0; idx < nprices; idx++) {
		arr[idx] = -1;
	}

	int result = _memoized_cut_rod_aux(prices, nprices, length, arr);
	free(arr);

	return result;
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
int bottom_up_cut_rod(const int *prices, const int nprices, int length)
{
// TODO 	
	return -1;
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
int extended_bottom_up_cut_rod(const int *prices, const int nprices, int length)
{
	// TODO           
	return -1;
}


/*
  PRINT-CUT-ROD-SOLUTION(p, n)

  (r, s) = EXTENDED-BOTTOM-UP-CUT-ROD(p, n)
  while n > 0
    print s[n]
    n = n - s[n]
 */
void print_cut_rod_solution(const int *prices, const int nprices, int length)
{
	// TODO  
}
