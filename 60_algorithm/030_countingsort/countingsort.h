/*
  countingsort

  Counting sort assumes that each of the n input elements is an
  integer in the range 0 to k, for some integer k. When k = O(n), the
  sort runs in Theta(n) time.

  In the code for counting sort, we assume that the input is an array
  A[1..n], and thus A.lenght = n. We require two other arrays: the
  array B[1..n] holds the sorted output, and the array C[0..k]
  provides temporary working storage.

  Counting sort has better performance than O(lg(n)) since it is NOT a
  comparison sort!

  REFERENCES
  - https://www.geeksforgeeks.org/counting-sort/
  - https://en.wikipedia.org/wiki/Counting_sort
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */
#ifndef COUNTINGSORT
#define COUNTINGSORT


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b);
int find_min_element(int *arr, int size);
int find_max_element(int *arr, int size);

void countingsort(int **arr, int size, int min_element, int max_element);


#endif /* COUNTINGSORT */
