/*
  implementation
 */

#include "binary-search-tree.h"

#include <stdint.h>
//#include <stdbool.h>

static void _tree_failure(const char* msg);
{
	perror(msg);
	exit(EXIT_FAILURE);
}

node_p tree_root(void)
{
	return _root;
}

void* tree_get_data(node_p)
{
	return node_p->data;
}

// TODO tree_inorder_walk    

/*
  TREE-SEARCH

  if x == NIL or k == x.key
    return x
  if k < x.key
    return TREE-SEARCH(x.left, k)
  else return TREE-SEARCH(x.right, k)
 */
void* tree_search(node_p node, const uint64_t key)
{
	if (!node) return NULL;
        if (key == node->key) return tree_get_data(node);
	if (key < node->key) return tree_search(node->left, key);
	else return tree_search(node->right, key);
}

/*
  ITERATIVE-TREE-SEARCH

  while x != NIL and k != x.key
    if k < x.key
      x = x.left
    else x = x.right
  return x
 */
void* tree_search_iterative(node_p node, const uint64_t key)
{
	while (node) {
		if (key == node->key) return tree_get_data(node);
		if (key < node->key) node = node->left;
		else node = node->right;
	}
	return NULL;
}

/*
  TREE-MINIMUM(x)

  while x.left != NIL
    x = x.left
  return x
 */
void* tree_minimum(node_p node)
{
	node_p ptr = node;
	while (NULL != (ptr = node->left)) {
		node = ptr;
	}
	return tree_get_data(node);
}

/*
  TREE-MAXIMUM(x)

  while x.right != NIL
    x = x.right
  return x
 */
void* tree_maximum(node_p node)
{
	node_p ptr = node;
	while (NULL != (ptr = node->right)) {
		node = ptr;
	}
	return tree_get_data(node);
}

// TODO tree_successor    

/*
  TREE-INSERT(T, z)

  y = NIL
  x = T.root
  while x != NIL
    y = x
    if z.key < x.key
      x = x.left
    else
      x = x.right
  z.p = y
  if y == NIL
    T.root = z
  elseif z.key < y.key
    y.left = z
  else
    y.right = z
 */
void tree_insert(uint64_t key, void* data)
{
	node_p y = NULL;
	node_p x = tree_root();
	while (x != NULL) {
		y = x;
		if (key < x->key)
			x = x->left;
		else
			x = x->right;
	}
	node_p z;
	z = malloc(sizeof(*z));
	if (!z) _tree_failure("allocation failed");
	z->key = key;
	z->data = data;
	z->parent = y;
	if (!y) _root = z; // tree was empty
	else if (z->key < y->key) y->left = z;
	else y->right = z;
}

// TODO _tree_transplant    
// TODO tree_delete    
void tree_delete(uint64_t key)
{
	// TODO free nodes
}


