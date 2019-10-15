// tree.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * tree order
 * 0 < nodes < max
//*/

#include "tree.h"

/*#########################################################################################
  tree functionality
##########################################################################################*/

/*
  insert()

  insert a new leaf
//*/
int insert(leaf *first, leaf *item)
{
  if (NULL == first) {
    fprintf(stderr, "ERROR: insert() - first is NULL\n");
    return -1;
  }
  if (NULL == item) {
    fprintf(stderr, "ERROR: insert() - item is NULL\n");
    return -1;
  }

  leaf *predecessor = first;
  if (NULL != find(first, &predecessor, item->data)) {
    // node already in tree - dropped
    fprintf(stderr, "ERROR: insert() - node \'%d\' is contained in the tree!\n",
            item->data);
    return -1;

  } else {
    // ok - node not found
    if (predecessor->data > item->data) {
      predecessor->left = item;
    } else if (predecessor->data < item->data) {
      predecessor->right = item;
    } else {
      // nothing - element already in the tree, security case here..
    }

    return 0;
  }
}


/*
  delete()

  deletes a node
  sets the pointer of the predecessor
//*/
int delete (leaf **item, leaf *predecessor)
{
  if (NULL == *item) {
    fprintf(stderr, "ERROR: delete() - item to delete was NULL\n");
    return -1;
  }
  if (NULL == predecessor) {
    fprintf(stderr,
            "ERROR: delete() - predecessor of the item to delete was NULL\n");
    return -1;
  }

  if (predecessor == *item) {
    fprintf(stderr,
            "ERROR: delete() - not deleted - predecessor and item \"%d\" are "
            "identical\n",
            (*item)->data);
    return -1;
  }

  if ((NULL != (*item)->left) || (NULL != (*item)->right)) {
    fprintf(stderr,
            "ERROR: delete() - the item %d is a edge and not a leaf node - "
            "only leafs can be deleted\n",
            (*item)->data);
    return -1;
  }

  if (predecessor->data > (*item)->data) {
    predecessor->left = NULL;
  } else if (predecessor->data < (*item)->data) {
    predecessor->right = NULL;
  }

  if (NULL != *item)
    free(*item);

  return 0;
}


/*
  find()

  find a leaf
//*/
leaf *find(leaf *first, leaf **predecessor, const unsigned int data)
{
  if (NULL == first) {
    fprintf(stderr, "ERROR: find() - first was NULL\n");
    return NULL;
  }

  leaf *item = NULL;
  leaf *tmp = NULL;

  item = first;
  while (1) {
    tmp = item;
    if (item->data > data) {
      if (0 > get_left_node(&item)) {
        *predecessor = tmp;
        return NULL;
      } else {
        *predecessor = tmp;
      }

    } else if (item->data < data) {
      if (0 > get_right_node(&item)) {
        *predecessor = tmp;
        return NULL;
      } else {
        *predecessor = tmp;
      }

    } else if (item->data == data) {
      return item;
    }
  }
}


/*
  delete all

  deletes the whole tree recursively
  doesn't delete the first node
//*/
int deleteall(leaf **item, leaf *predecessor)
{
  if (NULL == *item) {
    fprintf(stderr, "ERROR: deleteall() - first was NULL\n");
    return -1;
  }
  if (NULL == predecessor) {
    fprintf(stderr, "ERROR: deleteall() - aborting, predecessor was NULL\n");
    exit(EXIT_FAILURE);
  }

  if (NULL != (*item)->left) {
    leaf *left = (*item)->left;
    deleteall(&left, *item);
  }
  if (NULL != (*item)->right) {
    leaf *right = (*item)->right;
    deleteall(&right, *item);
  }
  delete (item, predecessor);

  return 0;
}


/*#########################################################################################
  general accessibility
##########################################################################################*/


/*
  find_node()

  finds a node by doing a left and right branch search
  returns node or NULL
//*/
leaf *find_node(leaf *first, const unsigned int data)
{
  if (NULL == first) {
    fprintf(stderr, "ERROR: find_node() - first node is NULL, aborting.\n");
    exit(EXIT_FAILURE);
  }

  leaf *dummy = first;
  if (first->data == data) {
    return first;
  }

  if (NULL != first->left) {
    return find(first->left, &dummy, data);
  }
  if (NULL != first->right) {
    return find(first->right, &dummy, data);
  }

  return NULL;
}


/*
  get_left_node()

  checks if lf is NULL
  avances the passed param to the left side
  returns -1 in case lf is NULL then
//*/
int get_left_node(leaf **item)
{
  if (NULL == *item) {
    fprintf(stderr, "ERROR: get_left_node() - lf is NULL, abort proggy!\n");
    return -1;
  }

  if (NULL == (*item)->left) {
    return -1;
  } else {
    *item = (*item)->left;
    return 0;
  }
}


/*
  get_right_node()

  checks if lf is NULL
  avances the passed param to the right side
  returns -1 in case lf is NULL then
//*/
int get_right_node(leaf **item)
{
  if (NULL == *item) {
    fprintf(stderr, "ERROR: get_right_node() - lf is NULL, abort proggy!\n");
    return -1;
  }

  if (NULL == (*item)->right) {
    return -1;
  } else {
    *item = (*item)->right;
    return 0;
  }
}


/*
  init_node()

  inits a node and writes it to the passed pointer
  allocates space for the node
  sets data
  inits left and right to NULL
//*/
int init_node(leaf **lf, const unsigned int data)
{
  if (NULL == (*lf = malloc(sizeof(**lf)))) {
    perror("ERROR: init_node() - malloc failed!");
    return -1;
  }

  (*lf)->data = data;
  (*lf)->left = NULL;
  (*lf)->right = NULL;

  return 0;
}


/*
  delete_nodes()

  deletes left and right branch
//*/
int delete_nodes(leaf **first)
{
  if (NULL == first) {
    fprintf(stderr, "ERROR: delete_nodes() - first node is NULL\n");
    return -1;
  }

  leaf *left = (*first)->left;
  if (NULL != left) {
    if (0 > deleteall(&left, *first)) {
      return -1;
    }
  }
  leaf *right = (*first)->right;
  if (NULL != right) {
    if (0 > deleteall(&right, *first)) {
      return -1;
    }
  }

  if (NULL != *first)
    free(*first);

  return 0;
}
