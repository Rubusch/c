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

/*
  dotty printer
 */
void print_dot(const char* filename)
{
	if (!filename)
		return;

	FILE *fp = fopen(filename, "w");
	fprintf(fp, "graph %s\n", "arr");
	fprintf(fp, "{\n");

	node_p ptr = _queue_first;
	if (1 == queue_size()) {
		// just one node
		fprintf(fp, "%d\n", ptr->data);
	} else {
		// print series
		node_p ptr_last = _queue_first;
		for (ptr = queue_successor(_queue_first); ptr != NULL; ptr = queue_successor(ptr)) {
			fprintf(fp, "%d -- %d;\n", ptr_last->data, ptr->data);
			ptr_last = ptr;
		}
	}

	fprintf(fp, "}\n");
	fclose(fp);
}


node_p queue_first()
{
	return _queue_first;
}

node_p queue_head()
{
 	return _queue_head;
}

int queue_empty()
{
	return (0 >= queue_size());
}

int queue_size()
{
	return _queue_size;
}

node_p queue_successor(node_p ptr)
{
	return ptr->next;
}

node_p queue_predecessor(node_p ptr)
{
	return ptr->prev;
}

int queue_enqueue(int data)
{
	if (queue_size() >= QUEUE_MAX_SIZE) {
		fprintf(stderr, "%s(): queue is full, dropped\n", __func__);
		return -1;
	}
	node_p ptr = NULL;
	ptr = malloc(sizeof(*ptr));
	if (!ptr) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	ptr->next = NULL;
	ptr->prev = NULL;
	ptr->data = data;

	if (queue_empty()) {
		_queue_first = ptr;
		_queue_head = _queue_first;
	} else {
		_queue_head->next = ptr;
		ptr->prev = _queue_head;
		_queue_head = ptr;
	}

	 _queue_size++;
	 return 0;
}

int queue_dequeue(int* data)
{
	node_p ptr = queue_first();
	if (NULL == ptr) {
		fprintf(stderr, "%s(): underflow\n", __func__);
		return -1;
	}
	if (0 == queue_size()) {
		_queue_first = NULL;
		_queue_head = NULL;
		fprintf(stderr, "ptr != NULL, but queue was empty!!!\n");
		return -2;
	}
	if (data) {
		*data = ptr->data;
	}
	node_p predecessor_node = queue_predecessor(ptr);
	node_p successor_node = queue_successor(ptr);
	if (predecessor_node) {
		predecessor_node->next = successor_node;
	} else {
		_queue_first = successor_node;
	}
	if (successor_node) {
		successor_node->prev = predecessor_node;
	} else {
		_queue_head = predecessor_node;
	}
	free(ptr);
	_queue_size--;

	return 0;
}

