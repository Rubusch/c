// quicksort.h
/*
//*/

#ifndef QUICKSORT
#define QUICKSORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

// in case data types max / min values
#include <limits.h>

// quicksort related
int interchange(void **, void **);
void quicksort(void *, void *);

// common
void sort(void **);
int compare(void *, void *);
int swap(void *, void *);
int isfirstelement(void *);
int islastelement(void *);
void *getnextelement(void *);
void *getprevelement(void *);

#endif
