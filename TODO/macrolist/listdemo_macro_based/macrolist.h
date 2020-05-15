// dl_list.h
/*
  a simple single-linked queue example
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LINKED_LIST
#define LINKED_LIST
#define DATASIZ 64
#endif

#define CREATE_LIST(NAME)						\
	typedef struct NAME {					\
		struct NAME *next;					\
		char data[DATASIZ];					\
	} NAME##_t;							\
									\
	static NAME##_t *first = NULL;				\
	static NAME##_t *last = NULL;					\
									\
	/* append a new element containing data */			\
	static int NAME##__append(const char data[DATASIZ])		\
	{								\
		NAME##_t *elem;					\
									\
		if (NULL == (elem = malloc(sizeof(*elem))))		\
			return -1;					\
									\
		elem->next = NULL;					\
		strcpy(elem->data, data);				\
									\
		if (!first) {						\
			first = elem;					\
		} else {						\
			last->next = elem;				\
		}							\
		last = elem;						\
									\
		return 0;						\
	}								\
									\
	/* remove one element containing data */			\
	static int NAME##__remove(const char data[DATASIZ])		\
	{								\
		NAME##_t *elem = first;				\
		NAME##_t *elem_before = NULL;				\
									\
		if (!first)						\
			return -1;					\
		while (elem) {						\
			if (0 == strcmp(data, elem->data)) {		\
				if (elem_before)			\
					elem_before->next = elem->next;	\
				free(elem);				\
				return 0;				\
			}						\
			elem_before = elem;				\
			elem = elem->next;				\
		}							\
									\
		return -1;						\
	}								\
									\
	/* remove all elements */					\
	static int NAME##__removeall()				\
	{								\
		NAME##_t *elem = first;				\
									\
		while (first) {						\
			elem = first->next;				\
			free(first);					\
			first = elem;					\
		}							\
									\
		return 0;						\
	}								\
									\
	/* print all elements' data */					\
	static void NAME##__print()					\
	{								\
		NAME##_t* elem = first;				\
									\
		if (!first) {						\
			puts ("empty");					\
			return;						\
		}							\
									\
		while (elem) {						\
			printf("%s(): '%s'\n", __func__, elem->data);	\
			elem = elem->next;				\
		}							\
	}


