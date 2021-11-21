/*
  bucketsort

  bucketsort works as follows:
  - Setup an array of initially empty buckets
  - SCATTER: Go over the original array, putting each object into its bucket
  - Sort each non-empty bucket
  - GATHER: Visit the buckets in order and put all elements back into the original array

  Bucketsort has better performance than O(lg(n)) since it is NOT a
  comparison sort!

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */
#ifndef BUCKETSORT
#define BUCKETSORT


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bucket {
	int* b;
	int size;
} bucket_t;

#define BUCKET_SIZE 10
#define NBUCKETS 3

int find_min_element(int *arr, int size);
int find_max_element(int *arr, int size);

void bucketsort(int *arr, int size);


#endif /* BUCKETSORT */
