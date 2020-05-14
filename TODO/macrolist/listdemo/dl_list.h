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
		last = elem;
	} else {
		last->next = elem;
	}

	return 0;
}

/* remove at the specified position */
static int elements__remove(const char data[DATASIZ])
{
	if (!first)
		return -1;

	
	// TODO
	return 0;
}

static int elements__removeall()
{
	// TODO
	return 0;
}


/* print all elements (debugging) */
static void elements__print()
{
	// TODO
}


#endif
