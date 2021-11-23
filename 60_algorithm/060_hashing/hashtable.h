/*
  hashtable

  index mapping (trivial hashing)

                     |      |
                     +------+
                   22|      |
                     +------+
  (key == 23) ---> 23|      |---> data for key[23]
                     +------+
                   24|      |
                     +------+
                     |      |

  a hash table supports the floowing operations
  - inserting
  - searching
  - deleting

  depending on the hashing function the access / search should be most
  efficient, best results are instead to iterate over elements or
  random access elements (if possible) to compute by formulae where a
  corresponding value lies.

  a good hash function should avoid hash collisions

  (the most simple example is as follows)


  REFERENCES
  - https://www.geeksforgeeks.org/index-mapping-or-trivial-hashing-with-negatives-allowed/
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX 10

int has[MAX+1][2];


int search(unsigned int key);
void insert(unsigned int arr[], int size);


#endif /* HASHTABLE_H */
