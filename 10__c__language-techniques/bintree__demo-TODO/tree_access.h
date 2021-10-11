// tree_access.h
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * functions to be modified due to the tree implementation
//*/

#ifndef TREE_ACCESS
#define TREE_ACCESS

#include "lotharlib/mathtools.h"
#include "tree/tree.h"
//#include "lotharlib/fo.h" // TODO: rm

// maximum number of nodes
#define MAX_NODES 65535

// maximum number of data value for a node
#define MAX_DATA_NUMBER 999

// maximum level of deepness in tree
// TODO: sizeof(long int) * 8
#define MAX_LEVEL 10

// painting
#define DATA_DIGITS 3

int quit(leaf **, unsigned int *);
int init_root(leaf **, unsigned int *, unsigned int);
int populate_tree(leaf *, unsigned int *, const unsigned int);
int add_element(leaf *, unsigned int *, const unsigned int);
int find_element(leaf *, leaf *, const unsigned int);
int delete_element(leaf *, unsigned int *, const unsigned int);
int delete_tree(leaf **, unsigned int *);
int write_tree(leaf *);

// generating tree to write to file
int get_max_level(leaf *, unsigned int);
int data_to_string(char *, const unsigned int, const unsigned int);
int init_path(char *, const unsigned int, const unsigned int);
int get_data_by_path(leaf *, char *, const unsigned int, char *,
		     const unsigned int, const unsigned int);
int get_paint_mask_per_row(char *, const unsigned int, const unsigned int,
			   const unsigned int);
int write_tree(leaf *);

// writing to file
int get_write_file_pointer(FILE **, char[FILENAME_MAX]);
int write_linewise(FILE *, char *, const unsigned long int);
int close_stream(FILE **);

#endif
