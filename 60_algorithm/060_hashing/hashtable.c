/*
  hashtable
 */

#include "hashtable.h"

static int has[MAX+1][2];

int search(unsigned int key)
{
	if (has[key][0] == 1) {
		return 1;
	} else {
		return 0;
	}
}

void insert(unsigned int arr[], int size)
{
	for (int idx = 0; idx < size; idx++) {
		has[arr[idx]][0] = 1;
	}
}
