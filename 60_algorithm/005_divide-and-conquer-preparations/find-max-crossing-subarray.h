/*
  find maximum crossing subarray

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */
#ifndef FIND_MAXIMUM_CROSSING
#define FIND_MAXIMUM_CROSSING


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
