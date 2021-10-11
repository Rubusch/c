/*
  2-dim-array: pointer to pointer
  
  a structure pointer to pointer is not necessary the same like a 2-dim-array -
why? the size of the second arrays refered by the first pointer can vary,
therefore the number of all elements in the structure is not necessary the
multiplication of the size of array A by array B. Furthermore, since the
pointers work with allocated space the size is completely dynamic! It will be
set up at runtime of the program (statical == set up at beginning of a program)

  General talking about pointers of pointers there are differences in the types
of the first and second pointer. This is the same with pointers in
arrays-of-arrays, since they are also just pointers, only the refered memory is
a statical one.

  Example:
  char** ptr = variable; // means ptr->ptr, we'll call them 1. poitner and 2.
pointer

  The type of the first pointer is long unsigned int - why?
  Because the first pointer points to an address where we have stored the
address of the second pointer, thus it's type is the size of an address number
which is unsigned long int (=64 bit).

  The type of the second (in general: only the type of the last pointer) is the
named type, here "char" - it points to somewhere where a char is stored (8 bit).

  this is important, since storing an 64 bit value in a variable of 8 bit will
only store one part, the rest will disappear, reading out that value gives
something completely diferent (a common error - therefore the compiler gives
warinings about implicit type conversions! Allways debug your warnings!!!).
//*/

// given inits
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENT_COUNT 10
#define ELEMENT_LENGTH 8

// forward declaration of function
void output(char **);

int main(int argc, char **argv)
{
	// Definition and allocation of pointer to pointers
	// first pointer:
	char **arr = NULL;
	if (NULL == (arr = calloc(ELEMENT_COUNT, sizeof(*arr)))) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	// second pointer:
	int cnt;
	for (cnt = 0; cnt < ELEMENT_COUNT; ++cnt) {
		if (NULL ==
		    (arr[cnt] = calloc(ELEMENT_LENGTH, sizeof(**arr)))) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}
	}

	// initialization of pointer to pointers
	char str[6];
	memset(str, '\0', 6);
	for (cnt = 0; cnt < ELEMENT_COUNT; ++cnt) {
		// adding the number to the content of str ;)
		memset(str, '\0', 6);
		sprintf(str, "str %1d", cnt);
		strncpy(arr[cnt], str, 6);
	}

	// Pass and call of a function with pointer to pointers
	output(arr);

	// free second pointer (to the elements)
	for (cnt = 0; cnt < ELEMENT_COUNT; ++cnt) {
		if (NULL != arr[cnt]) {
			free(arr[cnt]);
		}
	}

	// free first pointer
	if (NULL != arr) {
		free(arr);
	}

	printf("READY.");
	return 0;
}

// Definition of a funcion with pointer to pointers
void output(char **arr)
{
	// CAUTION: here the size of the pointer to pointers is unknown - has to be
	// passed as param!!!
	printf("\nSize of the array is: %i - FAILS!\n", (sizeof arr));

	// print out
	int idx;
	int jdx;
	for (idx = 0; idx < ELEMENT_COUNT; ++idx) {
		printf("%i. element:\t", idx);
		for (jdx = 0; jdx < (strlen(arr[idx]) + 1); ++jdx) {
			printf("%s\'%c\'", ((0 == jdx) ? ("") : (", ")),
			       arr[idx][jdx]);
		}
		printf("\n");
	}
	printf("\n");
}
