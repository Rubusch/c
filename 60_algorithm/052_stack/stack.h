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

/*
  uses linkedlist base
*/
#include "linkedlist.h"

node_p stack_first();
node_p stack_head();
int stack_empty();
int stack_size();
node_p stack_successor(node_p ptr);
node_p stack_predecessor(node_p ptr);
node_p stack_search(int data);
node_p stack_maximum();
node_p stack_minimum();

void stack_push(int data);
int stack_pop();


#endif /* STACK_H */
