/*
  QUEUE - a FIFO container

    in ---.
          |
          V
     +---------+
     +---------+
     +---------+
     +---------+
     +---------+
          |
          '---> out
 */

#include "queue.h"


node_p queue_first() { return list_first(); }
node_p queue_head() { return list_head(); }
int queue_empty() { return list_empty(); }
int queue_size() { return list_size(); }
node_p queue_successor(node_p ptr) { return list_successor(ptr); }
node_p queue_predecessor(node_p ptr) { return list_predecessor(ptr); }
node_p queue_search(int data) { return list_search(data); }
node_p queue_maximum() { return list_maximum(); }
node_p queue_minimum() { return list_minimum(); }

void queue_enqueue(int data)
{
	list_insert(data);
}

int queue_dequeue()
{
	node_p ptr = queue_first();
	if (NULL == ptr) {
		fprintf(stderr, "%s(): underflow\n", __func__);
		return -1;
	}

	int data = ptr->data;
	list_delete(ptr);
	return data;
}



