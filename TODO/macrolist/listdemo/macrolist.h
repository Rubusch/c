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



typedef struct elements {
	struct elements *next;
	char data[DATASIZ];
} elements_t;

static elements_t *elements__first = NULL;
static elements_t *elements__last = NULL;

/* append a new element containing data */
static int elements__append(const char data[DATASIZ])
{
	elements_t *elem;

	if (NULL == (elem = malloc(sizeof(*elem))))
		return -1;

	elem->next = NULL;
	strcpy(elem->data, data);

	if (!elements__first) {
		elements__first = elem;
	} else {
		elements__last->next = elem;
	}
	elements__last = elem;

	return 0;
}

/* remove one element containing data */
static int elements__remove(const char data[DATASIZ])
{
	elements_t *elem = elements__first;
	elements_t *elem_before = NULL;

	if (!elements__first)
		return -1;
	while (elem) {
		if (0 == strcmp(data, elem->data)) {
			if (elem_before)
				elem_before->next = elem->next;
			else
				elements__first = elem->next;
			free(elem);

			return 0;
		}
		elem_before = elem;
		elem = elem->next;
	}

	return -1;
}

/* remove all elements */
static int elements__removeall()
{
	elements_t *elem = elements__first;

	while (elements__first) {
		elem = elements__first->next;
		free(elements__first);
		elements__first = elem;
	}
	elements__first = NULL;
	elements__last = NULL;

	return 0;
}

/* print all elements' data */
static void elements__print()
{
	elements_t* elem = elements__first;

	if (!elements__first) {
		puts ("empty");
		return;
	}

	while (elem) {
		printf("%s(): '%s'\n", __func__, elem->data);
		elem = elem->next;
	}
}

#endif
