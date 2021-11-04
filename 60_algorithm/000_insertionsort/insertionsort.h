// insertionsort.h
/*
//*/

#ifndef INSERTIONSORT
#define INSERTIONSORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// insertion sort specific
void insert(void *);

void sort(void **);
int compare(void *, void *);
int swap(int *elem_a, int *elem_b);
int isfirstelement(void *);
int islastelement(void *);
void *getnextelement(void *);
void *getprevelement(void *);

#endif
