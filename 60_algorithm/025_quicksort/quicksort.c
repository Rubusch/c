/*
  quicksort
 */

#include "quicksort.h"

#include "time.h"


/*
  utility function
 */
void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
  PARTITION(A, p, r)

  x = A[r]
  i = p - 1
  for j = p to r - 1
    if A[j] <= x
      i = i + 1
      exchange A[i] with A[j]
  exchange A[i + 1] with A[r]
  return i + 1

  NB: low := p, high := r, pivot := q
 */
int partition(int arr[], int low, int high)
{
	int pivot = arr[high];
	int idx = low - 1;
	for (int jdx = low; jdx < high; jdx++) {
		if (arr[jdx] <= pivot) {
			idx++; // increment index of the smaller element
			swap(&arr[idx], &arr[jdx]);
		}
	}
	swap(&arr[idx+1], &arr[high]);
	return idx + 1;
}

/*
  QUICKSORT(A, p, r)

  if p < r
    q = PARTITION(A, p, r)
    QUICKSORT(A, p, q - 1)
    QUICKSORT(A, q + 1, r)

  NB: low := p, high := r, pivot := q
 */
void quicksort(int arr[], int low, int high)
{
	int pivot;
	if (low < high) {
		pivot = partition(arr, low, high);
		quicksort(arr, low, pivot - 1);
		quicksort(arr, pivot + 1, high);
	}
}

/*
  RANDOMIZED-PARTITION(A, p, r)

  i = RANDOM(p, r)
  exchange A[r] with A[i]
  return PARTITON(A, p, r)
 */
int randomized_partition(int arr[], int low, int high)
{
	srand(time(0));
	int upper = high;
	int lower = low;

	int idx = (rand() % (upper - lower + 1)) + lower;
	swap(&arr[high], &arr[idx]);
	return partition(arr, low, high);
}

/*
  RANDOMIZED-QUICKSORT(A, p, r)

  q = RANDOMIZED-PARTITION(A, p, r)
  RANDOMIZED-QUICKSORT(A, p, q-1)
  RANDOMIZED-QUICKSORT(A, q+1, r)
 */
void randomized_quicksort(int arr[], int low, int high)
{
	int pivot;
	if (low < high) {
		pivot = randomized_partition(arr, low, high);
		randomized_quicksort(arr, low, pivot - 1);
		randomized_quicksort(arr, pivot + 1, high);
	}
}

/*
  TAIL-RECURSIVE-QUICKSORT(A, p, r)

  while p < r
    // Partition and sort left to subarray
    q = PARTITION(A, p, r)
    TAIL-RECURSIVE-QUICKSORT(A, p, q-1)
    p = q + 1

  NB: The classic QUICKSORT algorithm contains two recursive calls to
  itself. After QUICKSORT calls PARTITION, it recursively sorts the
  left subarray and then it recursively sorts the right subarray. The
  second recursive call in QUICKSORT is not really necessary; we can
  avoid it by using an iterative control structure. This technique,
  called _tail recursion_, is provided automatically by good
  compilers.
 */
void tail_recursive_quicksort(int arr[], int low, int high)
{
	int pivot;
	while (low < high) {
		pivot = partition(arr, low, high);
		tail_recursive_quicksort(arr, low, pivot-1);
		low = pivot + 1;
	}
}

