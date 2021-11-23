/*
  linkedlist

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct node_s {
	struct node_s* next;
	struct node_s* prev;
	int data;
} node_t;

typedef node_t* node_p;

/*
  NB: NEVER INIT GLOBALS HERE!

  when setting first to NULL, the linker results in a double definition
*/
node_p _list_first;
node_p _list_head;
int _list_size;

/* to be adjusted, further entries will be dropped */
#define LIST_MAX_SIZE 10

void print_dot(const char* filename);

node_p list_first();
node_p list_head();
int list_empty();
int list_size();
node_p list_successor(node_p ptr);
node_p list_predecessor(node_p ptr);
void list_insert(int data);
void list_delete(node_p ptr);
node_p list_search(int data);
node_p list_maximum();
node_p list_minimum();


#endif /* LINKEDLIST_H */
