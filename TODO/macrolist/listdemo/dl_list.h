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
} element_t;

static element_t *first = NULL;
static element_t *last = first;


/* append at the end of list */
static int append(element_t *elem_before, const char data[DATASIZ])
{
	element_t *elem;
	if (!first)
		elem = first;

	if (!elem = alloc(sizeof(*elem)))
		return -1;

	strcpy(elem->data, data);
	elem->next = NULL;

	last->next = elem;

	return 0;
}

/* remove at the specified position */
static int discard(element_t **elem)
{
	// TODO
	return 0;
}

/* print all elements (debugging) */
static void printall()
{
	// TODO
}

#endif
