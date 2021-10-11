// tree_control.h
/*
  functions to be modified due to the tree implementation
//*/

#ifndef TREE_ACCESS
#define TREE_ACCESS

#include "tools.h"
#include "tree_backend.h"

// tree basic accessibility
int quit(unsigned int *);
int init_root(unsigned int *);
int populate_tree(unsigned int *, const unsigned int);
int add_element(unsigned int *);
int find_element();
int delete_leaf(unsigned int *);
int delete_tree(unsigned int *);

// writing to file and printing tree
int init_path(char *, const unsigned int, const unsigned int);
int get_data_by_path(char *, const unsigned int, char *, const unsigned int,
		     const unsigned int);
int get_paint_mask_per_row(unsigned int *, const unsigned int,
			   const unsigned int, const unsigned int);
int write_tree();

// misc
int delete_element();
int backup_element();
int backup_tree();

// traverser
int traverse_pre_order(unsigned int *);
int traverse_post_order(unsigned int *);
int traverse_in_order(unsigned int *);

// rotation
int rotate_left();
int rotate_right();

#endif
