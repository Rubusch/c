// shakersort.c
/*
  Shaker sort is in-place (doesn't need further ressources proportional to the
amount of elements to sort).

  Shaker sort is stable (an alphabetic order of elements, equal by another key,
e.g. birth dates sorted another time by that second key, remains in the same
alphabetic order).

  shakersort is a form of bubblesort - the only difference is that
  after reading the elements upwards, and restarting at the first element
  the elements will be read up and downwards alternally.
//*/

// for the element type
#include "main.h"

// header
#include "shakersort.h"

/*
  sorting function - this function should work completely abstracted from the
data type to sort
//*/
void sort(void **first)
{
	void *tmp = *first;

	// check if first is only one element
	// TODO: if(isfirstelement(*first) && islastelement(*first)
	if ((NULL == getnextelement(*first)) &&
	    (NULL == getprevelement(*first))) {
		return;
	}

	// start sorting
	char done = 0;
	tmp = *first;
	do {
		// consists in one time up and one time down
		done = 1;

		// up
		void *next = NULL;
		do {
			tmp = getnextelement(tmp);
			if (islastelement(tmp)) {
				break;
			}

			next = getnextelement(tmp);
			if (compare(tmp, next)) {
				swap(tmp, next);
				done = 0;
			}
		} while (!islastelement(tmp));

		// down
		void *prev = NULL;
		do {
			tmp = getprevelement(tmp);
			if (isfirstelement(tmp)) {
				break;
			}

			prev = getprevelement(tmp);
			if (compare(prev, tmp)) {
				swap(prev, tmp);
				done = 0;
			}
		} while (!isfirstelement(tmp));
	} while (!done);
	*first = tmp;

	return;
}

/*
  comparator, takes the content of the two elements to compare
//*/
int compare(void *arg_a, void *arg_b)
{
	element_t *elem_a = (element_t *)arg_a;
	element_t *elem_b = (element_t *)arg_b;

	unsigned int a = elem_a->data;
	unsigned int b = elem_b->data;

	if (a > b)
		return 1;
	else
		return 0;
}

/*
  swaps the elements, the first counter needs to be the iterator index
//*/
int swap(void *elem_a, void *elem_b)
{
	element_t *a = (element_t *)elem_a;
	element_t *b = (element_t *)elem_b;

	/*
    swapping within a double linked list, needs to redo 6 links
  //*/
	a->next = b->next; // can be NULL!
	b->prev = a->prev; // can be NULL!

	if (b->next)
		b->next->prev = a;
	b->next = a;

	if (a->prev)
		a->prev->next = b;
	a->prev = b;

	return 0;
}

/*
  check if the element is the first element
//*/
int isfirstelement(void *arg)
{
	element_t *tmp = (element_t *)arg;

	if (NULL == tmp->prev)
		return 1;
	else
		return 0;
}

/*
  check if the element is the last element
//*/
int islastelement(void *arg)
{
	element_t *tmp = (element_t *)arg;

	if (NULL == tmp->next)
		return 1;
	else
		return 0;
}

/*
  return the next element
//*/
void *getnextelement(void *arg)
{
	if (arg == NULL)
		return NULL;
	element_t *tmp = (element_t *)arg;

	return tmp->next;
}

/*
  returns the previous element
//*/
void *getprevelement(void *arg)
{
	if (arg == NULL)
		return NULL;

	element_t *tmp = (element_t *)arg;

	return tmp->prev;
}
