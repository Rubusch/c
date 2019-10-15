// tree_backend.h
/*
  Implement and change the needed functions and variables (marked with XXX)
  to handle the access to the underlaying tree implementation
//*/

#ifndef TREE_BACKEND
#define TREE_BACKEND

// maximum number of nodes
#define MAX_NODES 65535

// maximum number of data value for a node
#define MAX_DATA_NUMBER 999

// number of nodes - digits
#define NUM_OF_NODES_DIGITS 4

// maximum level of deepness in tree
#define MAX_LEVEL 10

#include "tree_control.h"

// XXX - insert the tree implementation!
#include "tree/tree.h"

// root node
int backend_create_root();
int backend_delete_root();
leaf *backend_get_root();

// root_clone
int backend_create_root_clone();
int backend_delete_root_clone();
leaf *backend_get_root_clone();

// nodes
leaf *backend_create_node();
int backend_insert_node();
int backend_free_node(leaf **);
int backend_delete_node(leaf **);
int backend_delete_leaf(leaf **, leaf *);
int backend_find_and_delete_node(leaf *);
int backend_delete_all();

// data
unsigned int backend_get_data(leaf *);
unsigned int backend_generate_data();
leaf *backend_set_data(leaf *);
leaf *backend_set_data_automatically(leaf *);

// traverser
leaf *backend_traverse_in_order(leaf *);
leaf *backend_traverse_post_order(leaf *);
leaf *backend_traverse_pre_order(leaf *);

// navegation
leaf *backend_find_node(leaf *);
leaf *backend_get_left_node(leaf *);
leaf *backend_get_right_node(leaf *);
leaf *backend_get_upper(leaf *);
unsigned int backend_get_max_level();

// node pointer
leaf *backend_get_node_ptr();
int backend_set_node_ptr(leaf *);
int backend_free_node_ptr();

// misc
int backend_print_data();
int backend_copy(leaf *);

// TODO:
leaf *backend_get_leftest();
leaf *backend_get_rightest();
int backend_backup_node(leaf *);
int backend_backup_tree(leaf *);
int backend_left_rotation(leaf *);
int backend_right_rotation(leaf *);

#endif
