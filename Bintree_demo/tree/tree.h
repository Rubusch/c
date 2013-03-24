// tree.h
/*
  tree implementation header
//*/

#ifndef TREE
#define TREE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_OF_NODES_DIGITS 4

typedef struct tree_leaf{

  // ptr: lhs 
  struct tree_leaf* left;

  // ptr: rhs
  struct tree_leaf* right;

  // data
  unsigned int data;
}leaf;


// specific tree abstraction

// tree functionality
int insert(leaf*, leaf*);
int delete(leaf**, leaf*);
leaf* find(leaf*, leaf**, const unsigned int);
int deleteall(leaf**, leaf*);

// tree tool methods - to be modified, depending on the tree
leaf* find_node(leaf*, const unsigned int);
int get_left_node(leaf**); 
int get_right_node(leaf**); 
int init_node(leaf**, const unsigned int);
int delete_nodes(leaf**); 

#endif
