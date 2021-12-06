/*
  red black tree

  In addition to the requirements imposed on a binary search tree the
  following properties must be satisfied by a red–black tree:

  1. Every node is either RED or BLACK
  2. The root node is BLACK (RED if no tree but only a single node)
  3. Every leaf (NIL) is BLACK
  4. If a node is red, then both its children are black
  5. For each node, all simple paths from the node to descendant
     leaves contain the same number of black nodes

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
  - Sedgewick [2008]
 */

#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// debug printing
#define DEBUG 1

typedef enum {
	BLACK = 0,
	RED = 1,
} color_t;

typedef struct node_s {
	struct node_s *parent;
	struct node_s *left;
	struct node_s *right;

	uint64_t key;
	color_t color;
	void *data;
} node_t;
typedef struct node_s* node_p;


// public
node_p tree_root(void);
void* tree_get_data(node_p node);
void tree_print_dot(const char* filename, node_p node);

void* tree_search(node_p node, const uint64_t key);
void* tree_search_iterative(node_p node, const uint64_t key);
node_p tree_minimum(node_p node);
node_p tree_maximum(node_p node);
node_p tree_successor(node_p node);
node_p tree_predecessor(node_p node);

void red_black_left_rotate(node_p node);
void red_black_right_rotate(node_p node);
void red_black_insert_fixup(node_p node);
void red_black_insert(uint64_t key, void* data);
void* red_black_delete(node_p *deletee);

// private (access for testing interface)
void red_black_insert_fixup(node_p node);
void red_black_delete_fixup(node_p node, node_p parent);
void red_black_transplant(node_p node, node_p new_subtree);


#endif /* RED_BLACK_TREE_H */
