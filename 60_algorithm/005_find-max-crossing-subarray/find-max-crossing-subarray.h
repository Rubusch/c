/*
  Mergesort (recursive)

  1. Find the middle point to divide the array into two halves:
             middle q = p + (size - p) / 2

  2. Call mergeSort for first half:
             Call sort(arr, p, q)

  3. Call mergeSort for second half:
             Call sort(arr, q + 1, size)

  4. Merge the two halves sorted in step 2 and 3:
             Call merge(arr, p, q, size)

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
  - https://www.geeksforgeeks.org/merge-sort/
 */
#ifndef MERGESORT
#define MERGESORT


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct content_s {
	int max_left;
	int max_right;
	int sum;
} content_t;

void find_max_crossing_subarray(int arr[], int size, content_t *content);
void max_crossing_subarray(int arr[], int low, int mid, int high, content_t* content);

#endif
