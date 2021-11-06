/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

#include "fifo.h"

/* private */

static item_p first;
static item_p last;
static int size;

item_p fifo__new(content_t *content)
{
	item_p item = malloc(sizeof(item));
	if (item == NULL) {
		perror("allocation of new fifo element failed");
		exit(EXIT_FAILURE);
	}
	item->content = content;
	item->prev = NULL;
	item->next = NULL;

	return item;
}

/* public */

int fifo__size()
{
	return size;
}

item_p fifo__first()
{
	return first;
}

item_p fifo__last()
{
	return last;
}

item_p fifo__next(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->next;
}

item_p fifo__prev(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->prev;
}

void fifo__push(content_p content)
{
	if (first == NULL) {
		last = fifo__new(content);
		first = last;
	} else {
		item_p item = fifo__new(content);
		last->next = item;
		item->prev = last;
		last = last->next;
	}

	size++;
}

content_p fifo__pop()
{
	if (NULL == first)
		return NULL;

	item_p elem = first;
	content_p content = first->content;

	first = fifo__next(first);

	if (NULL != first)
		first->prev = NULL;
	else
		last = NULL;

	free(elem);

	size--;
	return content;
}
