// main.h
/*
  frontend to test sortings
//*/

#ifndef MAINSORTING
#define MAINSORTING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIST_SIZE 10

typedef struct listelement {
	struct listelement *next;
	struct listelement *prev;
	unsigned int value;
} element;

void printlist(void *);
void alloclist(void *, void *);
void freelist(void *);

#endif
