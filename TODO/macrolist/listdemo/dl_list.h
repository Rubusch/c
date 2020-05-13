// dl_list.h
/*
  A double linked list works like that:

  <-prev\
  ->element<-prev\
      \next->element<-prev\
               \next->element
                        \next->NULL
//*/

// TODO tests tests and tests... 

#ifndef DOUBLE_LINKED_LIST
#define DOUBLE_LINKED_LIST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATASIZ 64

typedef struct element {
  struct element *next;
  char *data;
} element_t;

/* append at the end of list */
static int append(element_t *);

/* remove at the specified position */
static int discard(element_t **);

/* print all elements (debugging) */
static int printall();

static element_t *first = NULL;
static element_t *last = NULL;


/*********************************************/

#endif
