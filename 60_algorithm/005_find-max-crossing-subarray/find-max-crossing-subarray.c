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

#include "find-max-crossing-subarray.h"

void find_max_crossing_subarray(int arr[], int size, content_t* content)
{
	max_crossing_subarray(arr, 0, size/2, size-1, content);
}


void max_crossing_subarray(int arr[], int low, int mid, int high, content_t* content)
{
	int left_sum = arr[mid]; // TODO check
	int sum = 0;
	int max_left = 0;
	int max_right = 0;

	for (int idx = mid; idx >= low; idx--) {
		sum = sum + arr[idx];
		if (sum > left_sum) {
			left_sum = sum;
			max_left = idx;
		}
	}

	int right_sum = arr[mid+1]; // TODO check
	sum = 0;
	for (int jdx = mid + 1; jdx <= high; jdx++) {
		sum = sum + arr[jdx];
		if (sum > right_sum) {
			right_sum = sum;
			max_right = jdx;
		}
	}

	content->max_left = max_left;
	content->max_right = max_right;
	content->sum = left_sum + right_sum;
}
