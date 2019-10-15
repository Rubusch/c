// queue_item.h

#ifndef QUEUE_ITEM
#define QUEUE_ITEM

#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 16

struct queue_item {
  struct queue_item *next;
  char content[MAX_LENGTH];
};
typedef struct queue_item item;
static item *first = NULL;

item *getNewItem(char *);
void addItem(char *);
void removeItemAt(int);
void insertItemAt(int, char[]);
int queueSize();
item *getItemAt(int);
char *getContent(int);
int find(char[], int);

#endif
