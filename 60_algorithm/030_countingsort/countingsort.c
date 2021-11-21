/*
  countingsort
 */

#include "countingsort.h"

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
 */
void countingsort(int **arr, int size, int min_element, int max_element)
{
	int *input = *arr;
	int range = max_element - min_element;
	int *output;
	int *count;
	int idx;

	/* preparation */
	output = malloc(size * sizeof(*input));
	if (!output) {
		exit(EXIT_FAILURE);
	}
	count = malloc((range + 1) * sizeof(*input));
	if (!count) {
		exit(EXIT_FAILURE);
	}
	memset(count, 0, (range + 1) * sizeof(*count));

	/*
	  increment for each element in "input" the corresponding
	  index in "count"
	*/
	for (idx = 0; idx < size; idx++) {
		count[input[idx] - min_element]++;
	}

	/*
	  value in "input" is index in "count", augmenting the content
	  in count allows for re-referencing the index in "input",
	  thus for returning the sorted order
	*/
	for (idx = 1; idx < (range + 1); idx++) {
		count[idx] += count[idx-1];
	}

	/* obtaining sorted order */
	for (idx = size - 1; idx >= 0; idx--) {
		output[count[input[idx] - min_element] - 1] = input[idx];
		count[input[idx] - min_element]--;
	}

	/* writing result back */
	*arr = output;

	free(count);
	free(input);
}
