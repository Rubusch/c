/*
  heapsort
 */

#include "heapsort.h"

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


    put elements of 'arr' in heap order, in-place

    heapsort via heapify manages a tree structure (by each recursive
    call, one tree level is moved down - parent(i) is the caller)
 */
void heapify(int arr[], int size, int idx)
{
	/*
	  start is assigned the index in 'arr' of the last parent node

	  the last element in a 0-based array is at index size-1; find
	  the parent of that element

	  start = iParent(size-1)

	  NB: variants are bottom-up heap construction, and floyd's
	  heap construction
	 */
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

	for (int idx = size - 1; idx > 0; idx--) {
		swap(&arr[0], &arr[idx]);
		size--;
		heapify(arr, idx, 0); /* NB: usage of argument differs */
	}
}

/*
  HEAP-INCREASE-KEY

  if key < A[i]
    error "new key is smaller than current key"
  A[i] = key
  while i > 1 and A[PARENT(i)] > A[i]
    exchange A[i] with A[PARENT(i)]
    i = PARENT(i)

  NB: expects an ordered array
 */
int parent(int size)
{
	/*
	  we assume a pre-ordered structure

	  if the array is still not ordered, or ordered differently,
	  it will need to be heapsorted anyway and insertion can
	  happen ambigiously; if it was already ordered, then this
	  returned index will be the last index i.e. the highest value
	*/
	return size-1;
}
void heap_increase_key(int *arr, int idx, int key)
{
	if (key < arr[idx]) {
		fprintf(stderr, "new key is smaller than current key\n");
		return;
	}
	arr[idx] = key;
	while (idx > 0 && arr[parent(idx)] > arr[idx]) {
		swap(&arr[idx], &arr[parent(idx)]);
		idx = parent(idx);
	};
}

/*
  MAX-HEAP-INSERT

  A.heap-size = A.heap-size +1
  A[A.heap-size] = -00
  HEAP-INCREASE-KEY(A, A.heap-size, key)
 */
void heap_insert(int **arr, int* size, int key)
{
	if (!size)
		return;

	// realloc: extend used memory
	int *tmp = realloc(*arr, ((*size+1) * sizeof(**arr)));
	if (!tmp) {
		if (NULL != tmp) {
			free(tmp);
		}
		tmp = NULL;
		return;
	}
	tmp[*size] = -1;
	*arr = tmp;

	(*size)++;
	heap_increase_key(*arr, *size-1, key);
}

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
int heap_extract_max(int **arr, int *size)
{
	if (!arr)
		return -1;
	if (!size)
		return -1;

	if (*size < 1) {
		fprintf(stderr, "heap underflow\n");
	}

	int max = heap_maximum(*arr, *size);

	// realloc: reduce used memory
//	int *tmp = realloc(*arr, ((*size - 1) * sizeof(**arr)));
	int *tmp = realloc(*arr, ((*size - 1) * sizeof(*tmp)));
	if (!tmp) {
		if (NULL != tmp) {
			free(tmp);
		}
		tmp = NULL;
		return -1;
	}
//	tmp[*size] = -1;
// */
	*arr = tmp;
// */
	(*size)--;

	heapify(*arr, 1, *size-1);
//	heapify(*arr, 1, size-1);
// */
	return max;
}

/*
  HEAP-MAXIMUM
  return A[last]

  NB: expects an ordered array
 */
int heap_maximum(int *arr, int size)
{
	return arr[size-1];
}
