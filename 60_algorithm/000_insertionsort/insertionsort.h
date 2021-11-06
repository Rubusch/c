// insertionsort.h
/*
//*/

#ifndef INSERTIONSORT
#define INSERTIONSORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


// insertion sort specific
//void insert(int *);
void sort(int *first, int size);
int* is_greater(int *arg_a, int *arg_b);
int swap(int *elem_a, int *elem_b);
//int isfirstelement(void *);
//int islastelement(void *);
//void *getnextelement(void *);
//void *getprevelement(void *);

#endif
