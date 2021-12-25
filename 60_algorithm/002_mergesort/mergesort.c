/*
  mergesort

  mergesort represents the divide and conquer approach

  Divide: the problem into a number of subproblems that are smaller
          instances of the same problem.

  Conquer: the subproblems by solving them recursively. If the
           subproblem sizes are small enough, however just solve the
           subproblems in a straightforward manner.

  Combine: the solutions to the subproblems into the solution for the
           original problem.
 */

#include "mergesort.h"

void sort(int arr[], int size)
{
	divide(arr, 0, size-1);
}

/*
  notes

  arr: array to be sorted
  p: left section index
  q: right section index
  last: index of the last element in the array

  initial situation

      +-+-+-+-+-+
      |5|3|2|4|1| arr
      +-+-+-+-+-+
       0 1 2 3 4 [index]
       A   A   A
       |   |   |
       |   |   '--- last = 4
       |   '--- q = 2
       '--- p = 0
 */

void divide(int arr[], int p, int last)
{
	if (p >= last) {
		return;
	}
	int q = p + (last - p) / 2;

	// DIVIDE
	divide(arr, p, q);
	divide(arr, q + 1, last);

	// CONQUER
	mergesort(arr, p, q, last);
}

void mergesort(int arr[], int p, int q, int last)
{
	int idx, jdx, kdx;

	// create temp arrays
	const int nleft = q - p + 1;
	int left[nleft];

	const int nright = last - q;
	int right[nright];

	// populate temp arrays
	for (idx = 0; idx < nleft; idx++) {
		left[idx] = arr[p + idx];
	}
	for (jdx = 0; jdx < nright; jdx++) {
		right[jdx] = arr[q + 1 + jdx];
	}

	// merge the temp arrays back - COMBINE
	idx = 0; // index left temp array
	jdx = 0; // index right temp array
	kdx = p; // index of the merged array
	while (idx < nleft && jdx < nright) {
		// at least two valid elements to compare - the actual sorting
		if (left[idx] <= right[jdx]) {
			arr[kdx] = left[idx];
			idx++;
		} else {
			arr[kdx] = right[jdx];
			jdx++;
		}
		kdx++;
	}
	// remaining elements of left[], if any
	while (idx < nleft) {
		arr[kdx] = left[idx];
		kdx++;
		idx++;
	}
	// else, copy the remaining elements of right[]
	while (jdx < nright) {
		arr[kdx] = right[jdx];
		kdx++;
		jdx++;
	}
}
