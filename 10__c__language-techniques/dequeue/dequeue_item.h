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


#define CONTENT_LENGTH 13

struct dequeue_item {
  struct dequeue_item *next;
  struct dequeue_item *prev;
  char content[CONTENT_LENGTH];
};
typedef struct dequeue_item item;

item *getNewItem(char[]);
void addItem(char[]);
void removeItemAt(int);
void insertItemAt(int, char[]);
int dequeueSize();
item *getItemAt(int);
char *getContentAt(int);
int find(char[], int);

#endif
