/*
  randomized algorithm

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */
#ifndef RANDOMIZED_ALGORITHMS
#define RANDOMIZED_ALGORITHMS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ARR_SIZE 200

typedef struct{
	int idx;
	int score;
} candidate_t;

int hire_assistant(candidate_t arr[ARR_SIZE], const int sufficient);
int randomized_hire_assistant(candidate_t arr[ARR_SIZE], const int sufficient);
int permute_by_sorting(candidate_t arr[ARR_SIZE], const int sufficient);
int randomize_in_place(candidate_t arr[ARR_SIZE], const int sufficient);
int on_line_maximum_hiring(candidate_t arr[ARR_SIZE], int observation_period, const int sufficient);


#endif
