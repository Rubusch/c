/*
  heapsort

 */

#include "heapsort.h"


void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}


void print_structure(int arr[], int size, const char* filename)
{
	FILE *fp = fopen(filename, "w");

	fprintf(fp, "digraph %s\n", "arr");
	fprintf(fp, "{\n");

	if (1 == size) {
		// just one node
		fprintf(fp, "%d\n", arr[0]);
	} else {
		// print series
		int idx;
		for (idx = 1; idx < size; idx++) {
			fprintf(fp, "%d -> %d;", arr[idx-1], arr[idx]);
		}
	}

	fprintf(fp, "}\n");
	fclose(fp);
}


/*
  HEAPIFY(A, i)

  l = LEFT(i)
  r = RIGHT(i)
  if l < A.heap-size and A[l] > A[i]
    largest = l
  else
    largest = i
  if r < A.heap-size and A[r] > A[largest]
    largest = r
  if largest != i
    exchange A[i] with A[largest]
    HEAPIFY(A, largest)

 */
void heapify(int arr[], int size, int idx)
{
	int largest = idx;

	// where parent has floor((i-1) / 2)
	int left = 2 * idx + 1;
	int right = 2 * idx + 2;

	if (left < size && arr[left] > arr[idx]) {
		largest = left;
	}

	if (right < size && arr[right] > arr[largest]) {
		largest = right;
	}

	if (largest != idx) {
		swap(&arr[idx], &arr[largest]);
		heapify(arr, size, largest);
	}
}

/*
  BUILD-MAX-HEAP

  A.heap-size = A.length
  for i = [A.length/2] downto 0
    HEAPIFY(A, i)
 */
void max_heap(int arr[], int size)
{
	int idx;
	for (idx = size/2 - 1; idx >= 0; idx--) {
		heapify(arr, size, idx);
	}
}

/*
  HEAPSORT(A)

  BUILD-MAX-HEAP(A)
  for i = A.length downto 2
    exchange A[0] with A[i]
    A.heap-size = A.heap-size -1
    HEAPIFY(A, 1)
 */
void heapsort(int arr[], int size)
{
	max_heap(arr, size);

	int idx;
	for (idx = size - 1; idx > 0; idx--) {
		swap(&arr[0], &arr[idx]);
		size = size -1;
		heapify(arr, idx, 0);
	}
}

/*
  MAX-HEAP-INSERT

  A.heap-size = A.heap-size +1
  A[A.heap-size] = -00
  HEAP-INCREASE-KEY(A, A.heap-size, key)
 */
// TODO   

/*
  HEAP-EXTRACT-MAX

  if A.heap-size < 1
    error "heap underflow"
  max = A[0]
  A[0] = A[A.heap-size]
  A.heap-size = A.heap-size - 1
  MAX-HEAPIFY(A, 1)
  return max
 */
// TODO   

/*
  HEAP-INCREASE-KEY

  if key < A[i]
    error "new key is smaller than current key"
  A[i] = key
  while i > 1 and A[PARENT(i)] < A[i]
    exchange A[i] with A[PARENT(i)]
    i = PARENT(i)
 */
// TODO    

/*
  HEAP-MAXIMUM
  return A[0]
 */
// TODO   


