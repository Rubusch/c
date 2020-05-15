// dl_list.h
/*
  a simple single-linked queue example
//*/

#ifndef DOUBLE_LINKED_LIST
#define DOUBLE_LINKED_LIST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATASIZ 64

typedef struct element {
  struct element *next;
  char data[DATASIZ];
} elements_t;

static elements_t *first = NULL;
static elements_t *last = NULL;

/* append at the end of list */
static int elements__append(const char data[DATASIZ])
{
	elements_t *elem;

	if (NULL == (elem = malloc(sizeof(*elem))))
		return -1;

	elem->next = NULL;
	strcpy(elem->data, data);

	if (!first) {
		first = elem;
	} else {
		last->next = elem;
	}
	last = elem;

	return 0;
}

/* remove at the specified position */
static int elements__remove(const char data[DATASIZ])
{
	elements_t *elem = first;
	elements_t *elem_before = NULL;

	if (!first)
		return -1;

	while (elem) {
		if (0 == strcmp(data, elem->data)) {
			if (elem_before)
				elem_before->next = elem->next;
			free(elem);
			return 0;
		}
		elem_before = elem;
		elem = elem->next;
	}

	return -1;
}

static int elements__removeall()
{
	elements_t *elem = first;

	while (first) {
		elem = first->next;
		free(first);
		first = elem;
	}

	return 0;
}


/* print all elements (debugging) */
static void elements__print()
{
	elements_t* elem = first;

	if (!first) {
		puts ("empty");
		return;
	}

	while (elem) {
		printf("%s(): '%s'\n", __func__, elem->data);
		elem = elem->next;
	}
}


#endif
