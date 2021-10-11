// shellsort.c
/*
  Shellsort is in-place (doesn't need further ressources proportional to the
amount of elements to sort).

  Shellsort is NOT stable (an alphabetic order of elements, equal by another
key, e.g. birth dates sorted another time by that second key, doesn't need to be
the same alphabetic order).
  
  Shellsort is basically a type of an insertion sort. Moving elements makes
insertion sort ineffective, the shell sort concept tries to avoid that by
putting the elements into an 2-D-array.
//*/

// for the element type
#include "main.h"

// header
#include "shellsort.h"

void analyze_listsize(void *first, unsigned int *element_count,
		      unsigned int *power, unsigned int *row_count)
{
	// count elements
	puts("count elements"); // XXX
	void *tmp = first;
	*element_count = 0;
	while (NULL != tmp) {
		tmp = getnextelement(tmp);
		++(*element_count);
	}

	// get the power by 2
	puts("get the power"); // XXX
	*power = 1;
	unsigned int tmp_count = element_count;
	while ((tmp_count > 0) && (0 == (tmp_count = tmp_count % 2))) {
		++(*power);
		printf("intermediate_count: %d, power: %d\n", tmp_count,
		       *power); // xXX
	}

	// get length of rows to split
	puts("get the length of rows"); // XXX
	*row_count = 0;
	*row_count = 1 << *power;
	printf("row_count: %d\n", *row_count); // XXX
}

/*
  allocates space for array, split's the list and inits it
//*/
void setup_array(void *first, void **array, unsigned int *row_count)
{
	printf("setup_array()\n"); // XXX
	if (NULL == (*array = calloc(row_count, sizeof(*first)))) {
		perror("calloc failed.");
		freelist((void *)first);
		exit(EXIT_FAILURE);
	}

	// split the list and filling in the array of lists
	puts("split the list into array"); // XXX
	unisgned int idx = 0;
	void *tmp = first;

	// do a structure in parallel to provide the "last" pointers of the list array
	// TODO
	for (idx = 0; tmp != NULL; ++idx) {
		// cut each element
		// TODO

		// append to the list under the corresponding pointer of array
		// TODO: array[idx % row_count], tmp;

		// iterate
		tmp = getnextelement(tmp);
	}
}

/*
  free's the temporary array pointer
//*/
void cleanup_array(void **array, unsigned int row_count)
{
	// TODO free
	return 0;
}

/*
  TODO
//*/
void merge_array()
{
	printf("merge_array()\n"); // XXX
	// TODO
}

/*
  sorting
//*/
void sort(void **first)
{
	if (NULL == first)
		return;
	if (NULL == *first)
		return;
	if (isfirstelement(*first) && islastelement(*first))
		return;

	// 1. generate array
	void *array = NULL;
	unsigned int size_row = 0;
	setup_array(*first, &array, &size_row);

	// allocate space for the void*

	// loop..
	// 2. sort rows - use insertion sort!

	// 3. merge_array
	// ..loop until here

	// 4. free array void*
	cleanup_array(&array, size_row);
}

/*
  comparator
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
  swap elements
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
  is first element?
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
  is last element
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
  next one
//*/
void *getnextelement(void *arg)
{
	if (arg == NULL)
		return NULL;
	element_t *tmp = (element_t *)arg;

	return tmp->next;
}

/*
  previous one
//*/
void *getprevelement(void *arg)
{
	if (arg == NULL)
		return NULL;

	element_t *tmp = (element_t *)arg;

	return tmp->prev;
}
