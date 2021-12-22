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

#define create_stack(NAME, DATATYPE, MAXSIZE)				\
	typedef struct NAME##__node_s {					\
		struct NAME##__node_s* next;				\
		struct NAME##__node_s* prev;				\
		DATATYPE data;						\
	} NAME##__node_t;						\
									\
	typedef NAME##__node_t* NAME##__node_p;				\
									\
	NAME##__node_p _##NAME##__first;					\
	NAME##__node_p _##NAME##__head;					\
	int _##NAME##__size;						\
	static const int NAME##__MAX_SIZE = MAXSIZE;			\
									\
	static inline NAME##__node_p NAME##__first();			\
	static inline NAME##__node_p NAME##__head();			\
	static inline int NAME##__empty();				\
	static inline int NAME##__size();				\
	static inline NAME##__node_p NAME##__successor(NAME##__node_p ptr); \
	static inline NAME##__node_p NAME##__predecessor(NAME##__node_p ptr); \
	static inline int NAME##__push(DATATYPE data);			\
	static inline int NAME##__pop(DATATYPE *data);			\
									\
	NAME##__node_p NAME##__first()					\
	{								\
		return _##NAME##__first;				\
	}								\
									\
	NAME##__node_p NAME##__head()					\
	{								\
		return _##NAME##__head;					\
	}								\
									\
	int NAME##__empty()						\
	{								\
		return (0 >= NAME##__size());				\
	}								\
									\
	int NAME##__size()						\
	{								\
		return _##NAME##__size;					\
	}								\
									\
	NAME##__node_p NAME##__successor(NAME##__node_p ptr)		\
	{								\
		return ptr->next;					\
	}								\
									\
	NAME##__node_p NAME##__predecessor(NAME##__node_p ptr)		\
	{								\
		return ptr->prev;					\
	}								\
									\
	int NAME##__push(DATATYPE data)					\
	{								\
		if (NAME##__size() >= NAME##__MAX_SIZE) {		\
			fprintf(stderr, "%s(): queue is full, dropped\n", __func__); \
			return -1;					\
		}							\
		NAME##__node_p ptr = NULL;				\
		ptr = malloc(sizeof(*ptr));				\
		if (!ptr) {						\
			perror("allocation failed");			\
			exit(EXIT_FAILURE);				\
		}							\
		ptr->next = NULL;					\
		ptr->prev = NULL;					\
		ptr->data = data;					\
									\
		if (NAME##__empty()) {					\
			_##NAME##__first = ptr;				\
			_##NAME##__head = _##NAME##__first;		\
		} else {						\
			_##NAME##__head->next = ptr;			\
			ptr->prev = _##NAME##__head;			\
			_##NAME##__head = ptr;				\
		}							\
									\
		_##NAME##__size++;					\
		return 0;						\
	}								\
									\
	int NAME##__pop(DATATYPE *data)					\
	{								\
		NAME##__node_p ptr = NAME##__head();			\
		if (NULL == ptr) {					\
			fprintf(stderr, "%s(): underflow\n", __func__); \
			return -1;					\
		}							\
		if (0 == NAME##__size()) {				\
			_##NAME##__first = NULL;			\
			_##NAME##__head = NULL;				\
			fprintf(stderr, "ptr != NULL, but queue was empty!!!\n"); \
			return -2;					\
		}							\
		if (data) {						\
			*data = ptr->data;				\
		}							\
		NAME##__node_p predecessor_node = NAME##__predecessor(ptr); \
		NAME##__node_p successor_node = NAME##__successor(ptr);	\
		if (predecessor_node) {					\
			predecessor_node->next = successor_node;	\
		} else {						\
			_##NAME##__first = successor_node;		\
		}							\
		if (successor_node) {					\
			successor_node->prev = predecessor_node;	\
		} else {						\
			_##NAME##__head = predecessor_node;		\
		}							\
		free(ptr);						\
		_##NAME##__size--;					\
		return 0;						\
	}


#endif /* STACK_H */
