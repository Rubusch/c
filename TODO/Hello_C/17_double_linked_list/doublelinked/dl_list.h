// dl_list.h
/*
  A double linked list works like that:

  <-prev\
  ->element<-prev\
      \next->element<-prev\
               \next->element
                        \next->NULL
//*/

#ifndef DOUBLE_LINKED_LIST
#define DOUBLE_LINKED_LIST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATASIZ 64

typedef struct element {
  struct element *next;
  struct element *prev;
  char *data;
} element_t;

// external usage interface level
int removeall();
int appendelement(const char *, const unsigned int);
int insertelement(const char *, const unsigned int, const char *,
                  const unsigned int);
int removeelement(const char *, const unsigned int);
int printlist();

#endif
