// dl_list.h
/*
  a simple single-linked queue example
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include guard just to keep the constants global */
#ifndef LINKED_LIST
#define LINKED_LIST

#define DATASIZ 64

#endif /* LINKED_LIST */

#define CREATE_LIST(NAME)                                                      \
	typedef struct NAME {                                                  \
		struct NAME *next;                                             \
		char data[DATASIZ];                                            \
	} NAME##_t;                                                            \
                                                                               \
	static inline NAME##_t *NAME##__first = NULL;			\
	static inline NAME##_t *NAME##__last = NULL;			\
                                                                               \
	/* append a new element containing data */                             \
	static int NAME##__append(const char data[DATASIZ])		\
	{                                                                      \
		NAME##_t *elem;                                                \
                                                                               \
		if (NULL == (elem = malloc(sizeof(*elem))))                    \
			return -1;                                             \
                                                                               \
		elem->next = NULL;                                             \
		strcpy(elem->data, data);                                      \
                                                                               \
		if (!NAME##__first) {                                          \
			NAME##__first = elem;                                  \
		} else {                                                       \
			NAME##__last->next = elem;                             \
		}                                                              \
		NAME##__last = elem;                                           \
                                                                               \
		return 0;                                                      \
	}                                                                      \
                                                                               \
	/* remove one element containing data */                               \
	static int NAME##__remove(const char data[DATASIZ])                    \
	{                                                                      \
		NAME##_t *elem = NAME##__first;                                \
		NAME##_t *elem_before = NULL;                                  \
                                                                               \
		if (!NAME##__first)                                            \
			return -1;                                             \
		while (elem) {                                                 \
			if (0 == strcmp(data, elem->data)) {                   \
				if (elem_before)                               \
					elem_before->next = elem->next;        \
				else                                           \
					NAME##__first = elem->next;            \
				free(elem);                                    \
                                                                               \
				return 0;                                      \
			}                                                      \
			elem_before = elem;                                    \
			elem = elem->next;                                     \
		}                                                              \
                                                                               \
		return -1;                                                     \
	}                                                                      \
                                                                               \
	/* remove all elements */                                              \
	static int NAME##__removeall()                                         \
	{                                                                      \
		NAME##_t *elem = NAME##__first;                                \
                                                                               \
		while (NAME##__first) {                                        \
			elem = NAME##__first->next;                            \
			free(NAME##__first);                                   \
			NAME##__first = elem;                                  \
		}                                                              \
		NAME##__first = NULL;                                          \
		NAME##__last = NULL;                                           \
                                                                               \
		return 0;                                                      \
	}                                                                      \
                                                                               \
	/* print all elements' data */                                         \
	static void NAME##__print()                                            \
	{                                                                      \
		NAME##_t *elem = NAME##__first;                                \
                                                                               \
		if (!NAME##__first) {                                          \
			puts("empty");                                         \
			return;                                                \
		}                                                              \
                                                                               \
		while (elem) {                                                 \
			printf("%s(): '%s'\n", __func__, elem->data);          \
			elem = elem->next;                                     \
		}                                                              \
	}
