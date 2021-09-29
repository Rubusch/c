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

// TODO move out
#define MESSAGE_LENGTH 32

struct content_s {
	/* this can be literally anything */
	char msg[MESSAGE_LENGTH];
};
typedef struct content_s content_t;
typedef content_t* content_p;

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

/*
void dequeue__append(char[]); // TODO change to content_t
//void dequeue__insert_after(dequeue_item_t* prev, dequeue_item_t* item);
//dequeue_item_t* dequeue__find(content_t*);
//void dequeue__remove(dequeue_item_t* item); // first obtain element via find, then remove it
//dequeue_item_t* dequeue__next(dequeue_item_t* elem);
//dequeue_item_t* dequeue__prev(dequeue_item_t* elem);

item *getItemAt(int); // TODO rename

void removeItemAt(int); // TODO rm
void insertItemAt(int, char[]); // TODO rm
char *getContentAt(int); // TODO rm   
int find(char[], int); // TODO rm
// */


#endif
