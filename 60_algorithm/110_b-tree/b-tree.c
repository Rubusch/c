/*
 */

#include "b-tree.h"

// some additional headers needed for matrix_create()
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>


/* utils */

void btree_debug(const char* format, ...)
{
#ifdef DEBUG
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
#endif
}

void btree_failure(const char* format, ...)
{
	perror("failed!");
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}


btree_node_p _btree_allocate_node()
{
	btree_node_p ptr = NULL;
	ptr = malloc(sizeof(*ptr));
	if (!ptr) {
		btree_failure("%s [%d]: %s() - allocation failed",
			      __FILE__, __LINE__, __func__);
	}

	ptr->data = malloc(sizeof(*ptr->data));
// TODO size?
	if (!ptr->data) {
		btree_failure("%s [%d]: %s() - allocation failed",
			      __FILE__, __LINE__, __func__);
	}

	return ptr;
}

void _btree_write(btree_node_p node)
{
	// mock / dummy - use queue to "serialize" tree structure
	btree_debug("...write\n");
}

void _btree_read(btree_node_p node)
{
	// mock / dummy - use queue to "de-serialize" queue into b-tree
	btree_debug("...write\n");
}


// b-tree

/*
  B-TREE-SEARCH(x, k)

  i = 1
  while i <= x.n and k > x.key[i]
    i = i + 1
  if i <= x.n and k == x.key[i]
    return (x, i)
  elseif x.leaf
    return NIL
  else DISK-READ(x.c[i])
    return B-TREE-SEARCH(x.c[i], k)
*/
btree_node_p btree_search(btree_node_p node, int key)
{
	int idx;

	idx = 1;
	while (idx <= node->nkeys && key > node->key[idx]) {
		idx++;
	}
	if (idx <= node->nkeys && key == node->key[idx]) {
		return; /* TODO (node, idx) */                 
	} else if (node->leaf) {
		return NULL;
	} else {
		_btree_read(node->c[idx]); // TODO c?        
		return btree_search(node->c[idx], key); // TODO c?            
	}
}

/*
  B-TREE-CREATE(T)

  x = ALLOCATE-NODE()
  x.leaf = TRUE
  x.n = 0
  DISK-WRITE(x)
  T.root = x
*/
void btree_create()
{
	btree_node_p node = NULL;
	node = _btree_allocate_node();
	node->leaf = true;
	node->nkeys = 0;
	_btree_write(node);
	root = node;
}

/*
  B-TREE-SPLIT-CHILD(x, i)

  z = ALLOCATE-NODE()
  y = x.c[i]
  z.leaf = y.leaf
  z.n = t - 1
  for j = 1 to t - 1
    z.key[j] = y.key[j + t]
  if not y.leaf
    for j = 1 to t
      z.c[j] = y.c[j + t]
  y.n = t - 1
  for j = x.n + 1 downto i + 1
    x.c[j + 1] = x.c[j]
  x.c[i + 1] = z
  for j = x.n downto i
    x.key[j + 1] = x.key[j]
  x.key[i] = x.key[t]
  x.n = x.n + 1
  DISK-WRITE(y)
  DISK-WRITE(z)
  DISK-WRITE(x)
*/
void btree_split_child(btree_node_p node, int idx)
{
	btree_node_p z = _btree_allocate_node();
	btree_node_p y = node.child[idx];
	z->leaf = y.leaf;
	z.nkeys = TSIZE - 1;
	for (int jdx = 1; jdx < TSIZE - 1; jdx++) { // TODO jdx = 0    
		z.key[jdx] = y.key[jdx + TSIZE];
	}
	if (!y.leaf) {
		for (int jdx = 1; jdx < TSIZE; jdx++) {
			z.child[jdx] = y.child[jdx + TSIZE];
		}
	}
	y.nkeys = TSIZE - 1;
	for (int jdx = x.nkeys + 1; jdx >= idx + 1; jdx--) {
		x.child[jdx + 1] = x.child[jdx];
	}
	x.child[idx + 1] = z;
	for (int jdx = x.nkeys; jdx >= idx; jdx--) {
		x.key[jdx + 1] = x.key[jdx];
	}
	x.key[idx] = x.key[TSIZE]; // TODO TSIZE-1
	x.nkeys = x.nkeys + 1;
	_btree_write(y);
	_btree_write(z);
	_btree_write(x);
}

/*
  B-TREE-INSERT(T, k)

  r = T.root
  if r.n == 2t - 1
    s = ALLOCATE-NODE()
    T.root = s
    s.leaf = FALSE
    s.n = 0
    s.c[1] = r
    B-TREE-SPLIT-CHILD(s, 1)
    B-TREE-INSERT-NONFULL(s, k)
  else B-TREE-INSERT-NONFULL(r, k)
*/
void btree_insert(int key)
{
	btree_node_p r = root;
	if (r.nkeys == 2 * TSIZE - 1) {
		btree_node_p s = _btree_allocate_node();
		root = s;
		s.leaf = false;
		s.nkeys = 0;
		s.c[0] = r;
		btree_split_child(s, 1);
		btree_insert_nonfull(s, key); // TODO 
	} else {
		btree_insert_nonfull(r, key); // TOOD  
	}
}

/*
  B-TREE-INSERT-NONFULL(x, k)

  i = x.n
  if x.leaf
    while i >= 1 and k < x.key[i]
      x.key[i + 1] = x.key[i]
      i = i - 1
    x.key[i + 1] = k
    x.n = x.n + 1
    DISK-WRITE(x)
  else while i >= 1 and k < x.key[i]
      i = i - 1
    i = i + 1
    DISK-READ(x.c[i])
    if x.c[i].n == 2t - 1
      B-TREE-SPLIT-CHILD(x, i)
      if k > x.key[i]
        i = i + 1
    B-TREE-INSERT-NONFULL(x.c[i], k)
*/
void btree_insert_nonfull(btree_node_p node, int key)
{
	int idx = node->nkeys;
	if (node->leaf) {
		while (idx >= 1 && key < node->key[idx]) {
			node->key[idx + 1] = node->key[idx];
			idx--;
		}
		node->key[idx + 1] = key;
		node->nkeys++;
		_btree_write(node);
	} else {
		while (idx >= 1 && key < node->key[idx]) {
			idx--;
		}
		idx++;
		_btree_read(node->child[idx]);
		if (node->child[idx]->nkeys == 2 * TSIZE - 1) {
			btree_split_child(node, idx);
			if (key > node->key[idx]) {
				idx++;
			}
		}
		btree_insert_nonfull(node->child[idx], key);
	}
}
