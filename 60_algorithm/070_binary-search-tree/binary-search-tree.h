/*
  binary search tree

  binary-search-tree property: Let x be a node in a binary search
  tree. If y is a node in the left subtree of x, then y.key <=
  x.key. If y is a node in the right subtree of x, then y.key >=
  x.key.

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// debug printing
//#define DEBUG 1


typedef struct node_s {
	struct node_s *parent;
	struct node_s *left;
	struct node_s *right;

	uint64_t key;
	void *data;
} node_t;
typedef struct node_s* node_p;

node_p _root;

node_p tree_root(void);
void* tree_get_data(node_p);

void* tree_search(node_p node, const uint64_t key);
void* tree_search_iterative(node_p node, const uint64_t key);
void* tree_minimum(node_p node);
void* tree_maximum(node_p node);

// TODO 

void tree_insert(uint64_t key, void* data);
void tree_delete(uint64_t key);


#endif /* BINARY_SEARCH_TREE_H */
