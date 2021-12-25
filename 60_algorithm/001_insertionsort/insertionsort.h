/*
  Insertionsort

  To sort an array of size n in ascending order:

  1: Iterate from arr[1] to arr[n] over the array.

  2: Compare the current element (key) to its predecessor.

  3: If the key element is smaller than its predecessor, compare it to
     the elements before. Move the greater elements one position up to
     make space for the swapped element.

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
  - https://www.geeksforgeeks.org/insertion-sort/
 */

#ifndef INSERTIONSORT
#define INSERTIONSORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
void sort(int *first, int size);

#endif
