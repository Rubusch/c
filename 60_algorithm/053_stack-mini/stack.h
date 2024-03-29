/*
  stack

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef STACK_H
#define STACK_H

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

#define STACK_MAX_SIZE 10

void print_dot(const char* filename);

node_p stack_first();
node_p stack_head();
int stack_empty();
int stack_size();
node_p stack_successor(node_p ptr);
node_p stack_predecessor(node_p ptr);
int stack_push(int data);
int stack_pop(int* data);


#endif /* STACK_H */
