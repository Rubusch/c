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
  char *data;
} element_t;

/* append at the end of list */
static int append(element_t *)
{
	// TODO
	return 0;
}

/* remove at the specified position */
static int discard(element_t **)
{
	// TODO
	return 0;
}

/* print all elements (debugging) */
static void printall()
{
	// TODO
}

static element_t *first = NULL;
static element_t *last = NULL;

#endif
