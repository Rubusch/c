/*
  greedy methods

  greedy-choice property

  Here is where greedy algorithms differ from dynamic programming. In
  dynamic programming, we make a choice at each step, but the choice
  usually depends on the solutions to subproblems. Consequently, we
  typically solv dynamic-programming problems in a bottom-up manner,
  progressing from smaller subproblmes to larget
  subproblems. (Alternatively, we can solve them top down, but
  memoizing. Of course, even though the coe works top down, we still
  must solve the subproblems before making a choice).

  In a greedy algorithm, we make whatever choice seems best at the
  moment and then solve the subproblem that remains. The choice made
  by a greedy algorithm may depend on choices so far, but it cannot
  depend on any future choices or on the solutions to
  subproblems. Thus unlike dynamic programming, which solves the
  subproblems before making the first choice, a greedy algorithm makes
  its first choice before solving any subproblems. A
  dynamic-programmig algorithm proceeds bottom up, whereas a greedy
  strategy usually progresses in a top-down fashion, making one greedy
  choice after another, reducing each given problem instance to a
  smaller one.


  optimal substructure

  A problem exhibits optimal substructure if an optimal solution to
  the problem contains within it optimal solutions to
  subproblems. This property is a key ingredient of assessing the
  applicability of dynamic programming as well as greedy algorithms.

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef GREEDY_H
#define GREEDY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// some additional headers needed for matrix_create()
#include <string.h>
#include <stdarg.h>


// enable debug printout
# ifndef DEBUG
#  define DEBUG 1
# endif /* DEBUG */

// data
#define MAX_TREE_HT 100

typedef struct huffman_node_s {
    char data;
    unsigned freq;
    struct huffman_node_s *left, *right;
} huffman_node_t;
typedef huffman_node_t* huffman_node_p;

typedef struct huffman_heap_s {
    unsigned size;
    unsigned capacity;
    struct huffman_node_s** array;
} huffman_heap_t;
typedef huffman_heap_t* huffman_heap_p;

// utils
void greedy_debug(const char* format, ...);
void greedy_failure(const char* format, ...);
void swap(huffman_node_p *a, huffman_node_p *b);

// greedy
huffman_node_p new_node(char data, unsigned freq);
huffman_heap_p create_heap(unsigned capacity);
void huffman_heapify(huffman_heap_p heap, int idx);
int is_leaf(huffman_node_p root);

huffman_node_p build_huffman_tree(char data[], int freq[], int size);
void huffman(huffman_node_p *root, char data[], int freq[], int size);


#endif /* GREEDY_H */
