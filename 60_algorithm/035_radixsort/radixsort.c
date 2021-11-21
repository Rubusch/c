/*
  radixsort
 */

#include "radixsort.h"

#include "time.h"


/*
  utility function
 */

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
  COUNTING-SORT(A, B, k)

  let C[0..k] be a new array
  for i = 0 to k
    C[i] = 0
  for j = 1 to A.length
    C[A[j]] = C[A[j]] + 1
  // C[i] now contains the number of elements equal to i
  for i = 1 to k
    C[i] = C[i] + C[i-1]
  // C[i] now contains the number of elements less than or equal to i
  for j = A.length downto 1
    B[C[A[j]]] = A[j]
    C[A[j]] = C[A[j]] - 1



  RADIX SORT adaptation: count index is now "(input[idx] / exp) % 10",
  since count is now used to rank decimal digits
 */
void countingsort(int **arr, int size, int exp)
{
	int *input = *arr;
	int *output;
	int count[10] = { 0 }; // make decimal ranking
	int idx;

	/* preparation */
	output = malloc(size * sizeof(*input));
	if (!output) {
		exit(EXIT_FAILURE);
	}

	/*
	  increment for each element in "input" the corresponding
	  index in "count"
	*/
	for (idx = 0; idx < size; idx++) {
		count[(input[idx] / exp) % 10]++;
	}

	/*
	  value in "input" is index in "count", augmenting the content
	  in count allows for re-referencing the index in "input",
	  thus for returning the sorted order
	*/
	for (idx = 1; idx < 10; idx++) {
		count[idx] += count[idx - 1];
	}

	/* obtaining sorted order */
	for (idx = size - 1; idx >= 0; idx--) {
		output[count[(input[idx] / exp) % 10] - 1] = input[idx];
		count[(input[idx] / exp) % 10]--;
	}

	/* writing result back */
	*arr = output;

	free(input);
}

void radixsort(int **arr, int size)
{
	int max_element = find_max_element(*arr, size);

	for (int exp = 1; max_element / exp > 0; exp *= 10) {
		countingsort(arr, size, exp);
	}

}
