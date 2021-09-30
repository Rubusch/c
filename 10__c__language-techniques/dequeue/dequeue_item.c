// dequeue_item.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

#include "dequeue_item.h"


/* private */

static item_p first;
static item_p last;
static int size;

item_p dequeue__new(content_t* content)
{
	item_p item = malloc(sizeof(item));
	if (item == NULL) {
		perror("allocation of new dequeue element failed");
		exit(EXIT_FAILURE);
	}
	item->content = content;
	item->prev = NULL;
	item->next = NULL;

	return item;
};


/* public */

int dequeue__size()
{
	return size;
}

item_p dequeue__first()
{
	return first;
}

item_p dequeue__last()
{
	return last;
}

item_p dequeue__next(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->next;
};

item_p dequeue__prev(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->prev;
}

void dequeue__append(content_p content)
{
	if (first == NULL) {
		last = dequeue__new(content);
		first = last;
	} else {
		item_p item = dequeue__new(content);
		last->next = item;
		item->prev = last;
		last = last->next;
	}

	size++;
}

void dequeue__remove(item_p elem)
{
	/* NB: make sure item->content is free*d, and NULLed in advance */
//	item_p elem_before = NULL;
//	if (elem != first)
//		elem_before = dequeue__prev(elem);
	item_p elem_before = dequeue__prev(elem);

//	item_p elem_after = NULL;
//	if (elem != last)
//		elem_after = dequeue__next(elem);
	item_p elem_after = dequeue__next(elem);

	if (elem_before != NULL && elem_after != NULL) {
		elem_before->next = elem_after;
		elem_after->prev = elem_before;
	} else if (elem_before == NULL && elem_after != NULL) {
# ifdef DEBUG_DEQUEUE
		assert(elem == first);
		assert(elem != last);
# endif
		first = dequeue__next(first);
		first->prev = NULL;
	} else if (elem_after == NULL && elem_before != NULL) {
# ifdef DEBUG_DEQUEUE
		assert(elem != first);
		assert(elem == last);
# endif
		last = dequeue__prev(last);
		last->next = NULL;
	} else {
# ifdef DEBUG_DEQUEUE
		assert(elem == first);
		assert(elem == last);
		assert(elem->prev == NULL);
		assert(elem->next == NULL);
# endif
		/* remove: first == last */
		first = NULL;
		last = NULL;
	}
	free(elem);

	size--;
}

void dequeue__insert_after(item_p elem_before, content_p content)
{
	item_p item = dequeue__new(content);
	item_p elem_after;

	if (NULL != elem_before) {
		elem_after = dequeue__next(elem_before);
		elem_before->next = item;
	} else {
		if (NULL == dequeue__first()) {
			dequeue__append(content);
			return;
		}
		elem_after = dequeue__first();
	}
	item->prev = elem_before;

	if (NULL != elem_after) {
		elem_after->prev = item;
	}
	item->next = elem_after;

	size++;
}

item_p dequeue__find(content_p content)
{
	item_p item = first;
	while (item != NULL) {
		/* comparator, function to be rather part of content_t */
		if (!strcmp(item->content->msg, content->msg)) {
			int i = 0;
			for (i = 0; i < content->len; ++i)
				if (item->content->msg[i] != content->msg[i])
					break;
			if (i == content->len)
				return item;
		}
		item = item->next;
	}
	return NULL;
};
