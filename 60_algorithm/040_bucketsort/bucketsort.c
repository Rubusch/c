/*
  bucketsort
 */

#include "bucketsort.h"
#include "insertionsort.h"


int find_min_element(int *arr, int size)
{
	int min = arr[0];
	for (int idx = 1; idx < size; idx++) {
		if (arr[idx] < min) {
			min = arr[idx];
		}
	}
	return min;
}

int find_max_element(int *arr, int size)
{
	int max = arr[0];
	for (int idx = 1; idx < size; idx++) {
		if (arr[idx] > max) {
			max = arr[idx];
		}
	}
	return max;
}


/*
  BUCKET-SORT(A)

  n = A.length
  let B[0..n-1] be a new array
  for i = 0 to n-1
    make B[i] an empty list
  for i = 1 to n
    insert A[i] into list B[ {n A[i]} ]
  for i = 0 to n-1
    sort list B[i] with insertion sort
  concatenate the lists B[0], B[1],...,B[n-1] together in order
 */
void bucketsort(int *arr, int size)
{
	// preparation - data
	int max_element = find_max_element(arr, size);
	int min_element = find_min_element(arr, size);
	int bucket_range = (max_element - min_element) / NBUCKETS + 1;

	// preparation - memory
	bucket_t *bucket = NULL;
	bucket = malloc(NBUCKETS * sizeof(*bucket));
	if (!bucket) {
		exit(EXIT_FAILURE);
	}
	for (int idx=0; idx < NBUCKETS; idx++) {
		bucket[idx].b = malloc(BUCKET_SIZE * sizeof( *(bucket[idx].b) )); // BUCKET_SIZE is just a guessed pre-alloc
		if (!bucket[idx].b) {
			exit(EXIT_FAILURE);
		}
		memset(bucket[idx].b, 0, BUCKET_SIZE);
		bucket[idx].size = 0;
	}

	/* distribute the elements to the correct buckets */
	for (int idx = 0; idx < size; idx++) {
		int bucket_idx = (arr[idx] - min_element) / bucket_range;
		bucket[bucket_idx].b[bucket[bucket_idx].size] = arr[idx];
		bucket[bucket_idx].size++;
	}

	/* sort each of the buckets individually (could be parallelized) */
	for (int idx = 0; idx < NBUCKETS; idx++) {
		sort(bucket[idx].b, bucket[idx].size);
	}

	/* append the buckets */
	int index = 0;
	for (int idx = 0; idx < NBUCKETS; idx++) {
		for (int jdx = 0; jdx < bucket[idx].size; jdx++) {
			arr[index++] = bucket[idx].b[jdx];
		}
		free( bucket[idx].b ); // cleanup
	}

	// cleanup
	free(bucket);
}
