/*
  statistics
 */

#include "statistics.h"


void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

int find_min_element(const int *arr, const int size)
{
	int min = arr[0];
	for (int idx = 1; idx < size; idx++) {
		if (arr[idx] < min) {
			min = arr[idx];
		}
	}
	return min;
}

int find_max_element(const int *arr, const int size)
{
	int max = arr[0];
	for (int idx = 1; idx < size; idx++) {
		if (arr[idx] > max) {
			max = arr[idx];
		}
	}
	return max;
}

int find_median(const int *arr, const int size)
{
	int min_element = find_min_element(arr, size);
	int max_element = find_max_element(arr, size);
	int range = max_element - min_element + 1;

	int *count = calloc(range, sizeof(*arr));
	if (!count) {
		exit(EXIT_FAILURE);
	}
	memset(count, 0, sizeof(*count) * range);

	for (int idx = 0; idx < size; idx++) {
		(count[arr[idx] - min_element])++;
	}


	int median_idx = 0;
	for (int idx = 0; idx < range; idx++) {
		if (count[idx] > count[median_idx]) {
			median_idx = idx;
		}
	}

	return (median_idx + min_element);
}

