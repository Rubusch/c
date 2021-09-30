// fifo.h
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */
#ifndef FIFO
#define FIFO


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_FIFO 1

# ifdef DEBUG_FIFO
#include <assert.h>
# endif

#include "content.h"

struct fifo_s {
	struct fifo_s *next;
	struct fifo_s *prev;

	content_p content;
};
typedef struct fifo_s item_t;
typedef struct fifo_s* item_p;


int fifo__size();

item_p fifo__first();
item_p fifo__last();

item_p fifo__next(item_p elem);
item_p fifo__prev(item_p elem);

void fifo__push(content_p content);
content_p fifo__pop();

#endif
