// stack.h
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */
#ifndef STACK
#define STACK


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_STACK 1

# ifdef DEBUG_STACK
#include <assert.h>
# endif

#include "content.h"

struct stack_s {
	struct stack_s *next;
	struct stack_s *prev;

	content_p content;
};
typedef struct stack_s item_t;
typedef struct stack_s* item_p;


int stack__size();

item_p stack__first();
item_p stack__last();

item_p stack__next(item_p elem);
item_p stack__prev(item_p elem);

void stack__put(content_p content);
content_p stack__get();

#endif
