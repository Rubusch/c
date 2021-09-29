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


// TODO change char[] as content to content_t* type  
//item *getNewItem(char[]); 
int dequeue__size();
item_p dequeue__first();
item_p dequeue__last();

item_p dequeue__next(item_p elem);
item_p dequeue__prev(item_p elem);

void dequeue__append(content_p content);
void dequeue__remove(item_p item);

/*
//void dequeue__insert_after(dequeue_item_t* prev, dequeue_item_t* item);
//dequeue_item_t* dequeue__find(content_t*);
//

item *getItemAt(int); // TODO rename

void removeItemAt(int); // TODO rm
void insertItemAt(int, char[]); // TODO rm
char *getContentAt(int); // TODO rm   
int find(char[], int); // TODO rm
// */


#endif
