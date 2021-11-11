/*
  problems and approaches
 */

#include "randomized-algorithm.h"
#include <time.h>

/*
  Q: how to shuffle an array?
 */
void shuffle(candidate_t array[ARR_SIZE])
{
	int size = ARR_SIZE;
	candidate_t tmp;
	int idx, jdx;

	// random shuffle array
	srand(time(0));
	for (idx = 0; idx < size - 1; idx++) {
		jdx = idx + rand() / (RAND_MAX / (size - idx) + 1);
		tmp = array[jdx];
		array[jdx] = array[idx];
		array[idx] = tmp;
	}
}


/*
  HIRE ASSISTANT

  best = 0 // candidate 0 is the least qualified dummy candidate
  for i = 0 to n
    interview candidate i
    if candidate i is better than candidate best
      best = i
      hire candidate i
*/
int hire_assistant(candidate_t arr[ARR_SIZE], const int sufficient)
{
	int idx_best = 0;
	int idx;
	for (idx = 0; idx < ARR_SIZE; idx++) {
		if (arr[idx].score > arr[idx_best].score) {
			idx_best = arr[idx].idx;
			if (idx_best >= sufficient)
				break;
		}
	}
	return idx_best;
}

/*
  RANDOMIZED HIRE ASSISTANT

  randomly permute the list of candidates
  best = 0 // candidate 0 is the least qualified dummy candidate
  for i = 0 to n
    interview candidate i
    if candidate i is better than candidate best
      best = i
      hire candidate i
 */
int randomized_hire_assistant(candidate_t arr[ARR_SIZE], const int sufficient)
{
	// break orders by shuffling before
	shuffle(arr);
	return hire_assistant(arr, sufficient);
}

/*
  PERMUTE BY SORTING(A)

  n = A.length
  let P[1..n] be a new array
  for i = 0 to n
    P[i] = RANDOM(1, n^3)
  sort A, using P as sort keys
 */
int compare(const void* a, const void* b)
{
	return (((candidate_t*) b)->score - ((candidate_t*) a)->score);
}
int permute_by_sorting(candidate_t arr[ARR_SIZE], const int sufficient)
{
	const int size = ARR_SIZE;
	qsort(arr, size, sizeof(*arr), compare);
	return hire_assistant(arr, sufficient);
}

/*
  RANDOMIZE-IN-PLACE

  n = A.length
  for i = 0 to n
    swap A[i] with A[RANDOM(i,n)]

 Q: how to pick a random index, w/o repetition?
 */
int randomize_in_place(candidate_t arr[ARR_SIZE], const int sufficient)
{
	const int size = ARR_SIZE;
	int index_array[ARR_SIZE];
	int tmp;
	int idx, jdx;

	// setup index array, from 0..n
	for (idx = 0; idx < size; idx++) {
		index_array[idx] = idx;
	}

	// shuffle index array
	srand(time(0));
	for (idx = 0; idx < size - 1; idx++) {
		jdx = idx + rand() / (RAND_MAX / (size - idx) + 1);
		tmp = index_array[jdx];
		index_array[jdx] = index_array[idx];
		index_array[idx] = tmp;
	}

	for (idx = 0; idx < size; idx++) {
		// read random element
		jdx = index_array[idx];
		arr[idx].idx = jdx;
		arr[idx].score = arr[jdx].score;
	}

	return hire_assistant(arr, sufficient);
}

/*
  ON-LINE-MAXIMUM(k, n) [hiring, slight adjustments]

  bestscore = -00
  for i = 0 to k
    if score(i) > bestscore
      bestscore = score(i)
  for i = k + 1 to n
    if score(i) > bestscore
      return i
  return n
 */
int on_line_maximum_hiring(candidate_t arr[ARR_SIZE], int observation_period, const int sufficient)
{
	int best_idx = 0;
	const int size = ARR_SIZE;
	int idx;
	for (idx = 0; idx < observation_period; idx++) {
		if (arr[idx].score > arr[best_idx].score) {
			best_idx = idx;
		}
	}
	for (idx = observation_period + 1; idx < size; idx++) {
		if (arr[idx].score > arr[best_idx].score) {
			return idx;
		}
	}
	return best_idx;
}
