/*
  STACK - a LIFO container

   in ---. .---> out
         | |
         V |
     +---------+
     +---------+
     +---------+
     +---------+
     +---------+
 */

#include "stack.h"


node_p stack_first() { return list_first(); }
node_p stack_head() { return list_head(); }
int stack_empty() { return list_empty(); }
int stack_size() { return list_size(); }
node_p stack_successor(node_p ptr) { return list_successor(ptr); }
node_p stack_predecessor(node_p ptr) { return list_predecessor(ptr); }
node_p stack_search(int data) { return list_search(data); }
node_p stack_maximum() { return list_maximum(); }
node_p stack_minimum() { return list_minimum(); }

void stack_push(int data)
{
	list_insert(data);
}

int stack_pop()
{
	node_p ptr = list_head();
	if (NULL == ptr) {
		fprintf(stderr, "%s(): underflow\n", __func__);
		return -1;
	}

	int data = ptr->data;
	list_delete(ptr);
	return data;
}



