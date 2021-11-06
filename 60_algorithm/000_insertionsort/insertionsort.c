// insertionsort.c
/*
  Insertion sort is in-place (doesn't need further ressources proportional to
the amount of elements to sort).

  Insertion sort can be stable (an alphabetic order of elements, equal by
another key, e.g. birth dates sorted another time by that second key, remains
the same alphabetic order).
//*/

// header
#include "insertionsort.h"

void sort(int *first, int size)
{
	int idx=0, jdx=0;
	int *arr = first;
	int key;

	for(jdx=1; jdx < size; jdx++) {
		key = arr[jdx];
		idx = jdx -1;
		// compare the current element key to its predecessor arr[idx]
		while (idx >= 0 && (arr[idx] == max(arr[idx], key)) ) {
			arr[idx+1] = arr[idx];
			idx = idx -1;
		}
		arr[idx+1] = key;
	}
}
