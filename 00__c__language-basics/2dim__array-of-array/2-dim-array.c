/*
  2-dim-array

  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-May-01

  a real 2 dimensional array is an array (statical) of an array (statical)! It's
  a statical data structure and every array refered by in the first array has
  the SAME size!

  number of elements:
  {number of all emlements} == {number of elements in array A} x {number of
  elements in B}

  A pointer doesn't know if it points to an array of elements or, respectively,
  to more elements of allocated memory. A pointer only knows that address it
  points to AND the size of the value that is stored there (=the type of the
  pointer).

  Example:
  if a char* ptr get's initialized, it points to an address where one 'char'
  (=8 bit) are stored, therefore it's a pointer to 'char'.

  An array of an array, is - thought in pointer - a pointer to another pointer
  (and both pointing to static memory, means not allocated).
//*/


// given inits
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// allways avoid "magic numbers in code" and try to define "constant" values (in
// c these are "defines")
#define ELEMENT_COUNT 12
#define ELEMENT_LENGTH 10


// Forward declaration of the output function
// Attention: the size of the second pointer has to be specified already here!
void output(char[][ELEMENT_LENGTH]);


int main(int argc, char** argv){

  // Definition of a 2-dim-array
  char arr[ELEMENT_COUNT][ELEMENT_LENGTH] = {
    "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

  // print the size of 2-dim-array
  printf("Size of the array is: %ld, expected: %d\n", (sizeof(arr)/ELEMENT_LENGTH), ELEMENT_COUNT);
  printf("Number of elements in the array: %d x %d == %d\n", ELEMENT_COUNT, ELEMENT_LENGTH, ELEMENT_COUNT * ELEMENT_LENGTH);

  // Pass and call of the function with a 2-dim-array
  output(arr);

  puts("READY.\n");
  return 0;
}


// Definition of a funcion with 2-dim-array
void output(char arr[][ELEMENT_LENGTH])
{
#pragma GCC diagnostic push
  // gcc will warn about the sizeof(arr) which is clearly constant,
  // since we have arr[][ELEMENT_LENGTH]
  //
  // this demo should demonstrate language-basics, though, so the
  // warning is turned off for that purpose

  // CAUTION: here the "sizeof" of the static array is unknown - has to be passed as param!!!
#pragma GCC diagnostic ignored "-Wsizeof-array-argument"
  printf("\nSize of the array is: %li - FAILS!\n", (sizeof(arr)/ELEMENT_LENGTH));
#pragma GCC diagnostic pop

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
  printf("\n\nAs you can see the words don't fill up all the space that would be possible in an 2-dim-array\n");
  printf("therefore we can use another structure: a pointer to arrays or to other pointers!\n\n");
}

