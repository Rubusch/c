/*
  quicksort

  Divide: Partition (rearrange) the array A[p..r] into two (possibly
      empty) subarrays A[p..q-1] and A[q+1..r] such that each element
      of A[p..q-1] is less than or equal to A[q], which is, in turn,
      less than or equal to each element of A[q+1..r]. Compute the
      index q as part of this partitioning procedure.

  Conquer: Sort the two subarrays A[p..q-1] and A[q+1..r] by recursive
      calls to quicksort

  Combine: Because the subarrays are already sorted, no work is needed
      to compbine them: the entire array A[p..r] is now sorted

  REFERENCES
  - https://www.geeksforgeeks.org/heap-sort/
  - https://en.wikipedia.org/wiki/Quicksort
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */
#ifndef QUICKSORT
#define QUICKSORT


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b);

int partition(int arr[], int low, int high);
void quicksort(int arr[], int low, int high);
int randomized_partition(int arr[], int low, int high);
void randomized_quicksort(int arr[], int low, int high);
void tail_recursive_quicksort(int arr[], int low, int high);


#endif /* QUICKSORT */
