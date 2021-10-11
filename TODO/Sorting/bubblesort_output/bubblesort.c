// bubblesort.c
/*
  common bubblesort implementation
//*/

// for the element type
#include "main.h"

// header
#include "bubblesort.h"

#define DEBUG

/*
  sorting function
//*/
void *sort(void **arg)
{
	element **first = (element **)arg;
	element *tmp = *first;

	char done = 0;

	while (!done) {
#ifdef DEBUG
		printlist(*first); // XXX
#endif
		done = 1;
		tmp = *first;

		// while there is a next compare "tmp" and "tmp->next", in case swap
		while (tmp->next) {
#ifdef DEBUG
			printf("comparing: %02d and %02d", tmp->value,
			       tmp->next->value); // XXX
#endif
			if (compare((void *)tmp->value,
				    (void *)tmp->next->value)) {
#ifdef DEBUG
				printf(" swapping!");
#endif
				swap((void *)&tmp, tmp->next);
				done = 0;
			}

			// check for first element and set first
			if (NULL == tmp->prev) {
#ifdef DEBUG
				printf(" set new first!"); // XXX
#endif
				*first = tmp;
			}

#ifdef DEBUG
			printf("\n");
#endif

			// next element
			tmp = tmp->next;
		}
#ifdef DEBUG
		printf("\n");
#endif
	}

	return NULL;
}

/*
  comparator, takes the content of the two elements to compare
//*/
int compare(void *val_a, void *val_b)
{
	unsigned int a = (unsigned int)val_a;
	unsigned int b = (unsigned int)val_b;

	if (a > b)
		return 1;
	else
		return 0;
}

/*
  swaps the elements, the first counter needs to be the iterator index
//*/
int swap(void **elem_a, void *elem_b)
{
	element *a = *((element **)elem_a);
	element *b = (element *)elem_b;

	// counter
	element **tmp = (element **)elem_a;

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

	*tmp = b;

	return 0;
}
