/*
  heapsort - The heap can be represented by a binary tree or array.


  Why array based representation for Binary Heap?

  Since a Binary Heap is a Complete Binary Tree, it can be easily
  represented as an array and the array-based representation is
  space-efficient. If the parent node is stored at index I, the left
  child can be calculated by 2 * I + 1 and the right child by 2 * I +
  2 (assuming the indexing starts at 0).

  -> heapify: heapifies the array


  REFERENCES
  - https://www.geeksforgeeks.org/heap-sort/
  - https://en.wikipedia.org/wiki/Heapsort
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */
#ifndef HEAPSORT
#define HEAPSORT


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b);

void heapify(int arr[], int size, int idx);
void max_heap(int arr[], int size);
void heapsort(int arr[], int size);
void heap_insert(int **arr, int* size, int key);
void heap_increase_key(int *arr, int idx, int key);
int heap_extract_max(int **arr, int *size);
int heap_maximum(int *arr, int size);

#endif
