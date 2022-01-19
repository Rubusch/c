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


  Properties of the B-tree (CLRS terminology in brackets)

  1. Every node x has the following attributes:

     a.) x.nkeys, [x.n] the numer of keys currently stored in node x

     b.) x.key[], [x.key] the keys themselves stored in non-decreasing
         order (!), so that x.key[0] <= x.key[1] <= ... <=
         x.key[x.nkeys-1];

     c.) x.is_leaf, [x.leaf] a boolean value that is TRUE if x is a
         leaf and FALSE if x is an internal node

  -> All keys of a node are sorted in increasing order. The child
     between two keys k1 and k2 contains all keys in the range from k1
     and k2.


  2. [x.c], Each internal node x also contains x.nkeys + 1 pointers
     x.child[0], x.child[1], ..., x.child[x.nkeys - 1] to its
     children. Leaf nodes have no children, and so their child[i]
     attributes are undefined

     -> Number of children of a node is equal to the number of keys in
     it plus 1


  3. The keys x.key[i] separate the ranges of keys stored in each
     subtree: if key_i is any key stored in the subtree with root
     x.child[i], then

     key_1 <= x.key[0] <= key_2 <= x.key[1] <= ... <= x.key[x.nkeys-1]


  4. All leaves have the same depth, which is the tree's height h

     ->  All leaves are at the same level


  5. Nodes have lower and upper bounds on the number of keys they can
     contain.
     We express these bounds in terms of a fixed integer t >= 2 called
     the minimum degree of the B-tree:

     a.) Every node other than the root must have a t least t - 1
         keys.  Every internal node other than the root thus has at
         least t children. If the tree is nonempty, the root must have
         at least one key

     b.) Every node may contain ata most 2t - 1 keys. Therefore, an
         internal node may have at most 2t children. We say that a
         node is full if it contains exactly 2t - 1 keys

  -> A B-Tree is defined by the term minimum degree 't'.
     The value of t depends upon disk block size.

  -> Every node except root must contain at least t-1 keys. The root
     may contain minimum 1 key

  -> All nodes (including root) may contain at most 2*t - 1 keys

  -> B-Tree grows and shrinks from the root which is unlike Binary
     Search Tree. Binary Search Trees grow downward and also shrink
     from downward.

  -> Like other balanced Binary Search Trees, time complexity to
     search, insert and delete is O(log n).

  -> Insertion of a Node in B-Tree happens only at Leaf Node

  The simplest B-tree occurs when t = 2. Every internal node then has
  either 2, 3 or 4 children, and we have a 2-3-4 tree. In practice,
  however, much larger values of t yield B-trees with smaller height


  ---
  Example usages are disk read, disk write..


  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein] - CLRS
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
#define TSIZE 4

// data
typedef struct btree_node_s {
	int key[TSIZE];
	struct btree_node_s *child[TSIZE];
	bool is_leaf;
	int nkeys;
	content_t* data;
} btree_node_t;
typedef btree_node_t* btree_node_p;

// utils
void btree_debug(const char* format, ...);
void btree_failure(const char* format, ...);

// b-tree
void btree_create();
void btree_destroy();
btree_node_p btree_root();
btree_node_p btree_search(btree_node_p node, int key, int* idx_result);
void btree_split_child(btree_node_p node, int idx);
void btree_insert(int key);
void btree_insert_nonfull(btree_node_p node, int key);


#endif /* B_TREE_H */
