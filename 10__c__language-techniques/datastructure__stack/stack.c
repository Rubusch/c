/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

#include "stack.h"

/* private */

static item_p first;
static item_p last;
static int size;

item_p stack__new(content_t *content)
{
	item_p item = malloc(sizeof(item));
	if (item == NULL) {
		perror("allocation of new stack element failed");
		exit(EXIT_FAILURE);
	}
	item->content = content;
	item->prev = NULL;
	item->next = NULL;

	return item;
};

/* public */

int stack__size()
{
	return size;
}

item_p stack__first()
{
	return first;
}

item_p stack__last()
{
	return last;
}

item_p stack__next(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->next;
};

item_p stack__prev(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->prev;
}

void stack__put(content_p content)
{
	if (first == NULL) {
		last = stack__new(content);
		first = last;
	} else {
		item_p item = stack__new(content);
		last->next = item;
		item->prev = last;
		last = last->next;
	}

	size++;
}

content_p stack__get()
{
	if (NULL == first)
		return NULL;
	item_p elem = last;
	content_p content = last->content;
	last = stack__prev(last);
	if (NULL != last)
		last->next = NULL;
	else
		first = NULL;
	free(elem);
	size--;
	return content;
}
