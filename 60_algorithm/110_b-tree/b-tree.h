/*
  B-Tree (Bayer-Tree)

  In computer science, a B-tree is a self-balancing tree data
  structure that maintains sorted data and allows searches, sequential
  access, insertions, and deletions in logarithmic time. The B-tree
  generalizes the binary search tree, allowing for nodes with more
  than two children.

  In most of the other self-balancing search trees (like AVL and
  Red-Black Trees), it is assumed that everything is in main
  memory. To understand the use of B-Trees, we must think of the huge
  amount of data that cannot fit in main memory. When the number of
  keys is high, the data is read from disk in the form of blocks. Disk
  access time is very high compared to the main memory access
  time. The main idea of using B-Trees is to reduce the number of disk
  accesses.


  Properties of the B-tree

  1. All leaves are at the same level.

  2. A B-Tree is defined by the term minimum degree 't'. The value of
     t depends upon disk block size.

  3. Every node except root must contain at least t-1 keys. The root
     may contain minimum 1 key.

  4. All nodes (including root) may contain at most 2*t - 1 keys.

  5. Number of children of a node is equal to the number of keys in it
     plus 1.

  6. All keys of a node are sorted in increasing order. The child
     between two keys k1 and k2 contains all keys in the range from k1
     and k2.

  7. B-Tree grows and shrinks from the root which is unlike Binary
     Search Tree. Binary Search Trees grow downward and also shrink
     from downward.

  8. Like other balanced Binary Search Trees, time complexity to
     search, insert and delete is O(log n).

  9. Insertion of a Node in B-Tree happens only at Leaf Node.

  Example usages are disk read, disk write..


  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
  - https://www.geeksforgeeks.org/introduction-of-b-tree-2/
  - https://en.wikipedia.org/wiki/B-tree
 */

#ifndef B_TREE_H
#define B_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>


// enable debug printout
# ifndef DEBUG
#  define DEBUG 1
# endif /* DEBUG */

#define content_t int
#define T 4

// data
typedef struct btree_node_s {
	int[T] key;
	content_t* data;
	struct btree_node_s[T] next;
// TODO 	
	bool leaf;
	int nkeys;
	int size;
} btree_node_t;
typedef btree_node_t* btree_node_p;


static inline btree_node_p root = NULL;

// TODO     

// utils
void btree_debug(const char* format, ...);
void btree_failure(const char* format, ...);
// TODO     

// b-tree
void btree_create();
btree_node_p btree_search(btree_node_p node, int key);
// TODO     


#endif /* B_TREE_H */
