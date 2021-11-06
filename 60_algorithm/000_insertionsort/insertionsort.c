// insertionsort.c
/*
  Insertion sort is in-place (doesn't need further ressources proportional to
the amount of elements to sort).

  Insertion sort can be stable (an alphabetic order of elements, equal by
another key, e.g. birth dates sorted another time by that second key, remains
the same alphabetic order).
//*/

// for the element type  
//#include "main.h"  

// header
#include "insertionsort.h"

void insert(int* where, int *key)
{

/*
	void *prev = getprevelement(tmp);
	while (NULL != prev) {
		if (compare(prev, tmp)) { // sorting order
			swap(prev, tmp);
		} else {
			break;
		}
		prev = getprevelement(tmp);
	}
// */
}

void sort(int *first, int size)
{
	int idx=0, jdx=0;
	int *arr = first;
	int key;

	for(jdx=1; jdx < size; jdx++) {
		key = arr[jdx];
		idx = jdx -1;
		// compare the current element key to its predecessor arr[idx]
		while (idx >= 0 && (&arr[idx] == is_greater(&arr[idx], &key)) ) {
			arr[idx+1] = arr[idx];
			idx = idx -1;
		}
		arr[idx+1] = key;
	}
}

int* is_greater(int *arg_a, int *arg_b)
{
	if (*arg_a == max(*arg_a, *arg_b))
		return arg_a;
	else
		return arg_b;
}

int swap(int *elem_a, int *elem_b)
{
	if (!elem_a)
		return -1;
	if (!elem_b)
		return -1;

	int tmp = *elem_a;
	*elem_a = *elem_b;
	*elem_b = tmp;

	return 0;
}

int isfirstelement(void *arg)
{
/*
	element_t *tmp = (element_t *)arg;

	if (NULL == tmp->prev)
		return 1;
	else
// */
		return 0;
}

int islastelement(void *arg)
{
/*
	element_t *tmp = (element_t *)arg;

	if (NULL == tmp->next)
		return 1;
	else
// */
		return 0;
}

void *getnextelement(void *arg)
{
/*
	if (arg == NULL)
		return NULL;
	element_t *tmp = (element_t *)arg;

	return tmp->next;
// */
	return NULL;
}

void *getprevelement(void *arg)
{
/*
	if (arg == NULL)
		return NULL;

	element_t *tmp = (element_t *)arg;

	return tmp->prev;
// */
	return NULL;
}
