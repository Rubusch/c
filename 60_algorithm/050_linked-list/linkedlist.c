/*
  linkedlist
 */

#include "linkedlist.h"


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

	node_p ptr = _list_first;
	if (1 == list_size()) {
		// just one node
		fprintf(fp, "%d\n", ptr->data);
	} else {
		// print series
		node_p ptr_last = _list_first;
		for (ptr = list_successor(_list_first); ptr != NULL; ptr = list_successor(ptr)) {
			fprintf(fp, "%d -- %d;\n", ptr_last->data, ptr->data);
			ptr_last = ptr;
		}
	}

	fprintf(fp, "}\n");
	fclose(fp);
}


node_p list_first()
{
	return _list_first;
}

node_p list_head()
{
	return _list_head;
}

int list_empty()
{
	return (NULL == _list_first);
}

int list_size()
{
	return _list_size;
}

node_p list_successor(node_p ptr)
{
	return ptr->next;
}

node_p list_predecessor(node_p ptr)
{
	return ptr->prev;
}

void list_insert(int data)
{
	if (list_size() >= LIST_MAX_SIZE) {
		fprintf(stderr, "%s(): list is full, dropped\n", __func__);
		return;
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

	if (list_empty()) {
		_list_first = ptr;
		_list_head = _list_first;
	} else {
		_list_head->next = ptr;
		ptr->prev = _list_head;
		_list_head = ptr;
	}

	 _list_size++;
}

void list_delete(node_p ptr)
{
	if (!ptr)
		return;

	if (0 == list_size()) {
		_list_first = NULL;
		_list_head = NULL;
		return;
	}

	node_p predecessor_node = list_predecessor(ptr);
	node_p successor_node = list_successor(ptr);

	if (predecessor_node) {
		predecessor_node->next = successor_node;
	} else {
		// we are first
		_list_first = successor_node;
	}

	if (successor_node) {
		successor_node->prev = predecessor_node;
	} else {
		// we are head
		_list_head = predecessor_node;
	}

	free(ptr);
	_list_size--;
}

node_p list_search(int data)
{
	if (list_empty())
		return NULL;

	node_p ptr = NULL;
	for (ptr = _list_first; NULL != ptr; ptr = list_successor(ptr)) {
		if (data == ptr->data) {
			return ptr;
		}
	}
	return NULL;
}

node_p list_maximum()
{
	if (list_empty())
		return NULL;

	node_p ptr = NULL;
	node_p ptr_max = _list_first;
	for (ptr = _list_first; NULL != ptr; ptr = list_successor(ptr)) {
		if (ptr_max->data < ptr->data) {
			ptr_max = ptr;
		}
	}

	return ptr_max;
}

node_p list_minimum()
{
	if (list_empty())
		return NULL;

	node_p ptr = NULL;
	node_p ptr_min = _list_first;
	for (ptr = _list_first; NULL != ptr; ptr = list_successor(ptr)) {
		if (ptr_min->data > ptr->data) {
			ptr_min = ptr;
		}
	}

	return ptr_min;
}
