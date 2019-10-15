// selectionsort.h
/*
//*/

#ifndef SELECTIONSORT
#define SELECTIONSORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// selection sort specific
void *find_minimum(void *);
int exchange(void **, void **);

void sort(void **);
int compare(void *, void *);
int swap(void *, void *);
int isfirstelement(void *);
int islastelement(void *);
void *getnextelement(void *);
void *getprevelement(void *);

#endif
