/*
  queue

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct node_s {
	struct node_s* next;
	struct node_s* prev;
// TODO void* data;
	int data;
} node_t;

typedef node_t* node_p;

#define QUEUE_MAX_SIZE 10

void print_dot(const char* filename);

node_p queue_first();
node_p queue_head();
int queue_empty();
int queue_size();
node_p queue_successor(node_p ptr);
node_p queue_predecessor(node_p ptr);
int queue_enqueue(int data);
int queue_dequeue(int *data);


#endif /* QUEUE_H */
