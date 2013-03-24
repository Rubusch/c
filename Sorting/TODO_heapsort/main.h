// main.h
/*
  frontend to test sortings
//*/

#ifndef MAINSORTING
#define MAINSORTING

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// max limit
#define MAX_LIST_SIZE 10

// garbage collection
unsigned int global_list_size;

typedef struct listelement{
  struct listelement* next;
  struct listelement* prev;
  unsigned int data;
} element_t; 

void printlist(void*);
void alloclist(void*, void*);
void freelist(void*);

#endif
