/*
  2-dim-array

  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3

  This is kind of the most common structure, a pointer to arrays or an array of
  a pointer. Here is just one as example: a pointer to arrays.

  Conclusion:

  char** ptr; // has a first pointer (unsigned long int because of the address
  size!) and a second pointer to a value of the type itself, here a 'char'.

  Nowadays the size of addresses is 64 bit or unsigned long int!

  &ptr;   // the address of the first pointer itself (unsigned long int)
  ptr;    // the address of the second pointer (unsigned long int)
  *ptr;   // the content of the first pointer, the address of the char value
          // (unsigned long int)
  **ptr;  // the content of the second pointer: a char value (8 bit)
//*/


// given inits
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ELEMENT_COUNT 10
#define ELEMENT_LENGTH 8


// forwared declaration of a function with 2-dim-array
void output(char(*)[ELEMENT_LENGTH]);


int main(int argc, char** argv){

  // Definition of a pointer to arrays - Attention: use parenthesis!!!
  char (*arr)[ELEMENT_LENGTH];

  // Allocation of a pointer to arrays
  if( NULL == (arr = calloc(ELEMENT_COUNT, sizeof(*arr)))){
    perror("allocation failed");
    exit(EXIT_FAILURE);
  }

  // initialization of the pointer to arrays
  char str[6];
  int cnt;
  for(cnt = 0; cnt < ELEMENT_COUNT; ++cnt){
    memset(str, '\0', 6);
    // adding the number to the content of str ;)
    sprintf(str, "str %1d", cnt);
    strncpy(arr[cnt], str, 6);
  }

  // size of a pointer to arrays
  printf("Size of the array is: %li - FAILS due to the pointer!\n", (sizeof arr));

  // Pass and call of a function with pointer to array
  output(arr);

  // free
  if(NULL != arr) free(arr);

  puts("READY.\n");
  return 0;
}


// Definition of a funcion with pointer to arrays
void output(char (*arr)[ELEMENT_LENGTH])
{
  // CAUTION: here the size of the array is unknown - has to be passed as param!!!
  printf("\nSize of the array is: %li - FAILS due to pass!\n", (sizeof arr));

  // print out
  int idx;
  int jdx;
  for(idx = 0; idx < ELEMENT_COUNT; ++idx){
    printf("%i. element:\t", idx);
    for(jdx = 0; jdx < (strlen(arr[idx])+1); ++jdx){
      printf("%s\'%c\'", ((0 == jdx) ? ("") : (", ")) , arr[idx][jdx]);
    }
    printf("\n");
  }
  printf("\n");
}


/***
    Trick to undergo strictly set static arrays:

    int realarray[10];
    int *array = &realarray[-1];
***/


/***
    Can I use a void ** pointer to pass a generic pointer-to-pointer to a set of
    functions by reference?
***/

/*
  Not portably. There is no generic pointer-to-pointer type in C. void* acts as
  a generic pointer only because conversions are applied automatically when
  other pointer types are assigned to and from void*'s; these conversions cannot
  be performed (the correct underlying pointer type is unknown!!!) if an attempt
  is made to indirect upon a void** value which points at something other than a
  void*.
//*/


