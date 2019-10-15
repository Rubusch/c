// tree_backend.c
/*
  the interface functions needed to access the tree
  -> to be implemented according to the tree implementation
//*/

#include "tree_backend.h"

// XXX - static items to be set up
static leaf *root;
static leaf *root_clone; // TODO use
static leaf *node_ptr;


/*
  creates a new node and
  inits root with that node
//*/
int backend_create_root()
{
  if (NULL == (root = backend_set_data(backend_create_node()))) {
    return -1;
  }
  return 0;
}


/*
  deletes the root node
//*/
int backend_delete_root()
{
  if (root != NULL) {
    free(root);
    root = NULL;
    return 0;
  } else
    return -1;
}


/*
  returns the root node
//*/
leaf *backend_get_root() { return root; }


/*
  creates a new node and
  inits root with that node
//*/
int backend_create_root_clone()
{
  if (NULL == (root_clone = backend_set_data(backend_create_node()))) {
    return -1;
  }
  return 0;
}


/*
  deletes the root node
//*/
int backend_delete_root_clone()
{
  if (root_clone != NULL) {
    free(root_clone);
    root_clone = NULL;
    return 0;
  } else
    return -1;
}


/*
  returns the root node
//*/
leaf *backend_get_root_clone() { return root_clone; }


/*
  creates a new node to return
//*/
leaf *backend_create_node()
{
  leaf *item = NULL;
  if (0 > init_node(&item, 0)) {
    return NULL;
  }
  return item;
}


/*
  inserts an element to the tree
//*/
int backend_insert_node(leaf *item) { return insert(backend_get_root(), item); }


/*
  deletes an element directly
  only frees the memory!
//*/
int backend_free_node(leaf **item)
{
  if (NULL != *item) {
    free(*item);
    *item = NULL;
    return 0;
  }
  return -1;
}


/*
  deletes a node within the tree
  asks and uses node ptr directly do pass the pointer's address to the delete
function!
//*/
int backend_delete_node(leaf **item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return -1;
}


/*
  deletes a leaf in the tree
//*/
int backend_delete_leaf(leaf **item, leaf *prev_item)
{
  if (0 > delete (item, prev_item)) {
    return -1;
  }
  return 0;
}


/*
  finds and deletes an element in the tree
//*/
int backend_find_and_delete_node(leaf *item)
{
  // item to delete is root?
  if (item->data == backend_get_root()->data) {
    if (backend_get_left_node(backend_get_root())) {
      return -1;
    } else if (backend_get_right_node(backend_get_root())) {
      return -1;
    }
    backend_delete_root();
    return 0;
  }

  // ..or item to delete is NOT root
  leaf *delnode = backend_get_root();
  leaf *prevnode = backend_get_root();
  if (delnode->data == item->data) {
    if ((NULL == backend_get_left_node(delnode)) &&
        (NULL == backend_get_right_node(delnode))) {
      backend_delete_node(&item);
      return 0;
    } else {
      return -1;
    }
  } else if (NULL != (delnode = find((backend_get_root())->left, &prevnode,
                                     item->data))) {
    // ok found, next step: delete
  } else if (NULL != (delnode = find((backend_get_root())->right, &prevnode,
                                     item->data))) {
    // ok found, next step: delete
  } else {
    return -1;
  }

  if (0 > backend_delete_leaf(&delnode, prevnode)) {
    return -1;
  }

  return 0;
}


/*
  delete the entire structure
//*/
int backend_delete_all() { return delete_nodes(&root); }


/*
  returns the data stored in item
//*/
unsigned int backend_get_data(leaf *item) { return item->data; }


/*
  generates the data for a node
//*/
unsigned int backend_generate_data() { return random() % MAX_DATA_NUMBER; }


/*
  asks for the data and
  sets the data of an element to return
//*/
leaf *backend_set_data(leaf *item)
{
  if (item == NULL)
    return NULL;
  printf("a data value can have %d digits at most\n", DATA_DIGITS);
  readnumber(&item->data, DATA_DIGITS, "enter data value");
  return item;
}


/*
  generates data and
  sets the data of an element to return
//*/
leaf *backend_set_data_automatically(leaf *item)
{
  if (item == NULL)
    return NULL;
  item->data = backend_generate_data();
  return item;
}


/*
  in order traverser
  works as iterator and returns a pointer to the following next item
//*/
leaf *backend_traverse_in_order(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return NULL;
}


/*
  post order traverser
  works as iterator and returns a pointer to the following next item
//*/
leaf *backend_traverse_post_order(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return NULL;
}


/*
  pre-order traverser
  works as iterator and returns a pointer to the following next item
//*/
leaf *backend_traverse_pre_order(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return NULL;
}


/*
  finds and element in the tree
//*/
leaf *backend_find_node(leaf *item)
{
  return find_node(backend_get_root(), item->data);
}


/*
  returns the left elemenet in the tree
//*/
leaf *backend_get_left_node(leaf *item)
{
  if (0 > get_left_node(&item)) {
    return NULL;
  }
  return item;
}


/*
  returns the right element in the tree
//*/
leaf *backend_get_right_node(leaf *item)
{
  if (0 > get_right_node(&item)) {
    return NULL;
  }
  return item;
}


/*
  returns the upper elment in a double linked tree
//*/
leaf *backend_get_upper(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return NULL;
}


/*
  get_max_level()

  runs through the tree and finds the max tree level, recursively
  returns the current level
//*/
unsigned int backend_get_max_level()
{
  unsigned int level = get_max_level(backend_get_root(), 0);
  if (level > MAX_LEVEL)
    return MAX_LEVEL;
  else
    return level;
}


/*
  get pointer to an temporary element
  allocate space using the create node function!
//*/
leaf *backend_get_node_ptr() { return node_ptr; }


/*
  set the node pointer
  returns -1 if node_ptr was initializes to NULL
//*/
int backend_set_node_ptr(leaf *item)
{
  if (NULL == (node_ptr = item))
    return 0;
  else
    return -1;
}


/*
  frees the element of the node_ptr
//*/
int backend_free_node_ptr()
{
  if (NULL == node_ptr) {
    return -1;
  }
  free(node_ptr);
  node_ptr = NULL;
  return 0;
}


/*
  prints out / returns the data stored in a node
//*/
int backend_print_data(leaf *item)
{
  printf("data: %d\n", item->data);
  return 0;
}


/*
  copies the tree or part of the tree by the given root element
//*/
int backend_copy(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return -1;
}


/*
  returns the leftest element
//*/
leaf *backend_get_leftest()
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return NULL;
}


/*
  returns the rightest element
//*/
leaf *backend_get_rightest()
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return NULL;
}


/*
  takes a single node and places it in the backup tree with root_clone
//*/
int backend_backup_node(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return -1;
}


/*
  copies the entire subtree of the given parameter to the node root_clone
//*/
int backend_backup_tree(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return -1;
}


/*
  procedes a left rotation on a given element in the tree
//*/
int backend_left_rotation(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return -1;
}


/*
  procedes a right rotation on a given element in the tree
//*/
int backend_right_rotation(leaf *item)
{
  fprintf(stderr, "ERROR: NOT IMPLEMENTED\n");
  return -1;
}
