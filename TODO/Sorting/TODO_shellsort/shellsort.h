// shellsort.h
/*
//*/

#ifndef SHELLSORT
#define SHELLSORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// shellsort specific
void analyze_listsize(void *first, unsigned int *, unsigned int *,
                      unsigned int *);
void setup_array(void *, void **, unsigned int *);
void cleanup_array(void **, unsigned int);
void merge_array(); // TODO

void sort(void **);
int compare(void *, void *);
int swap(void *, void *);
int isfirstelement(void *);
int islastelement(void *);
void *getnextelement(void *);
void *getprevelement(void *);

#endif
