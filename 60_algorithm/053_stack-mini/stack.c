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

	node_p ptr = _stack_first;
	if (1 == stack_size()) {
		// just one node
		fprintf(fp, "%d\n", ptr->data);
	} else {
		// print series
		node_p ptr_last = _stack_first;
		for (ptr = stack_successor(_stack_first); ptr != NULL; ptr = stack_successor(ptr)) {
			fprintf(fp, "%d -- %d;\n", ptr_last->data, ptr->data);
			ptr_last = ptr;
		}
	}

	fprintf(fp, "}\n");
	fclose(fp);
}


node_p stack_first()
{
	return _stack_first;
}

node_p stack_head()
{
 	return _stack_head;
}

int stack_empty()
{
	return (0 >= stack_size());
}

int stack_size()
{
	return _stack_size;
}

node_p stack_successor(node_p ptr)
{
	return ptr->next;
}

node_p stack_predecessor(node_p ptr)
{
	return ptr->prev;
}

int stack_push(int data)
{
	if (stack_size() >= STACK_MAX_SIZE) {
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

	if (stack_empty()) {
		_stack_first = ptr;
		_stack_head = _stack_first;
	} else {
		_stack_head->next = ptr;
		ptr->prev = _stack_head;
		_stack_head = ptr;
	}

	_stack_size++;
	return 0;
}

int stack_pop(int *data)
{
	node_p ptr = stack_head();
	if (NULL == ptr) {
		fprintf(stderr, "%s(): underflow\n", __func__);
		return -1;
	}
	if (0 == stack_size()) {
		_stack_first = NULL;
		_stack_head = NULL;
		fprintf(stderr, "ptr != NULL, but queue was empty!!!\n");
		return -2;
	}
	if (data) {
		*data = ptr->data;
	}
	node_p predecessor_node = stack_predecessor(ptr);
	node_p successor_node = stack_successor(ptr);
	if (predecessor_node) {
		predecessor_node->next = successor_node;
	} else {
		_stack_first = successor_node;
	}
	if (successor_node) {
		successor_node->prev = predecessor_node;
	} else {
		_stack_head = predecessor_node;
	}
	free(ptr);
	_stack_size--;
	return 0;
}



