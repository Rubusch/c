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

/*
  uses linkedlist base
*/
#include "linkedlist.h"

int queue_empty();
int queue_size();
node_p queue_successor(node_p ptr);
node_p queue_predecessor(node_p ptr);
node_p queue_search(int data);
node_p queue_maximum();
node_p queue_minimum();

void queue_enqueue(int data);
int queue_dequeue();


#endif /* QUEUE_H */
