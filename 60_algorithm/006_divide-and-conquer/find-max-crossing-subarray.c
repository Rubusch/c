/*
 */

#include "find-max-crossing-subarray.h"


void find_max_crossing_subarray(int arr[], int size, content_t* content)
{
	divide(arr, 0, size-1, content);
}

void divide(int arr[], int low, int high, content_t* content)
{
	if (low == high)
		return;

	int mid = (low+high)/2;
	content_t left, right, cross;

	divide(arr, low, mid, &left);
	divide(arr, mid+1, high, &right);
	max_crossing_subarray(arr, low, mid, high, &cross);

	// MERGE
	if (left.sum >= right.sum && left.sum >= cross.sum) {
		content->low = left.low;
		content->high = left.high;
		content->sum = left.sum;
	} else if (right.sum >= left.sum && right.sum >= cross.sum) {
		content->low = right.low;
		content->high = right.high;
		content->sum = right.sum;
	} else {
		content->low = cross.low;
		content->high = cross.high;
		content->sum = cross.sum;
	}
}

void max_crossing_subarray(int arr[], int low, int mid, int high, content_t* content)
{
	int left_sum = arr[mid];
	int sum = 0;
	int max_left = 0;
	int max_right = 0;

	// CONQUER
	for (int idx = mid; idx >= low; idx--) {
		sum = sum + arr[idx];
		if (sum > left_sum) {
			left_sum = sum;
			max_left = idx;
		}
	}

	int right_sum = arr[mid+1];
	sum = 0;
	for (int jdx = mid + 1; jdx <= high; jdx++) {
		sum = sum + arr[jdx];
		if (sum > right_sum) {
			right_sum = sum;
			max_right = jdx;
		}
	}

	content->low = max_left;
	content->high = max_right;
	content->sum = left_sum + right_sum;
}
