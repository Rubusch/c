// dequeue_item.h
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */
#ifndef DEQUEUE
#define DEQUEUE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "content.h"

struct dequeue_item_s {
	struct dequeue_item_s *next;
	struct dequeue_item_s *prev;

	content_p content;
};
typedef struct dequeue_item_s item_t;
typedef struct dequeue_item_s* item_p;


int dequeue__size();

item_p dequeue__first();
item_p dequeue__last();

item_p dequeue__next(item_p elem);
item_p dequeue__prev(item_p elem);

void dequeue__append(content_p content);
void dequeue__remove(item_p item);

void dequeue__insert_after(item_p elem_before, content_p content);
item_p dequeue__find(content_p);


#endif
