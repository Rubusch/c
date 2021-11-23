/*
  linkedlist
 */

#include "linkedlist.h"


int list_empty()
{
	return (NULL == first);
}

int list_size()
{
	return size;
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
		first = ptr;
		head = first;
	} else {
		head->next = ptr;
		ptr->prev = head;
		head = ptr;
	}

	size++;
}

void list_delete(node_p ptr)
{
	if (!ptr)
		return;

	if (0 == size) {
		first = NULL;
		head = NULL;
		return;
	}

	node_p predecessor_node = list_predecessor(ptr);
	node_p successor_node = list_successor(ptr);

	if (predecessor_node) {
		predecessor_node->next = successor_node;
	} else {
		// we are first
		first = successor_node;
	}

	if (successor_node) {
		successor_node->prev = predecessor_node;
	} else {
		// we are head
		head = predecessor_node;
	}

	free(ptr);
	size--;
}

node_p list_search(int data)
{
	if (list_empty())
		return NULL;

	node_p ptr = NULL;
	for (ptr = first; NULL != ptr; ptr = list_successor(ptr)) {
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
	node_p ptr_max = first;
	for (ptr = first; NULL != ptr; ptr = list_successor(ptr)) {
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
	node_p ptr_min = first;
	for (ptr = first; NULL != ptr; ptr = list_successor(ptr)) {
		if (ptr_min->data > ptr->data) {
			ptr_min = ptr;
		}
	}

	return ptr_min;
}
