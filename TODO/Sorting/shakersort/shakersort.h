// shakersort.h
/*
  header to the bubblesort
//*/

#ifndef SHAKERSORT
#define SHAKERSORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sort(void **);
int compare(void *, void *);
int swap(void *, void *);
int isfirstelement(void *);
int islastelement(void *);
void *getnextelement(void *);
void *getprevelement(void *);

#endif
