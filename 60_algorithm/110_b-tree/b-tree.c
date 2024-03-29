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

/*
  INORDER-TREE-WALK(x)

  if x != NIL
    INORDER-TREE-WALK(x.left)
    print x.key
    INORDER-TREE-WALK(x.right)
 */
#ifdef DEBUG
static void _btree_print_key(btree_node_p node, char buf[64])
{
	char tmp[64]; memset(tmp, '\0', 64);
//fprintf(stderr, "%s [%d]: %s() - XXX buf '%s', nkeys %d\n",
//	__FILE__, __LINE__,  __func__, buf, node->nkeys);

	for (int idx=0; idx < node->nkeys && idx < 64/4; idx++) {
// NB: the debug printer is supposed just for small tree nkey sizes!
		if (0 == idx) {
			sprintf(tmp, "%d", node->key[idx]->val);
		} else {
			sprintf(tmp, "%s, %d", buf, node->key[idx]->val);
		}
		strcpy(buf, tmp);
	}
//fprintf(stderr, "%s [%d]: %s() - XXX buf %s, tmp %s\n",
//	__FILE__, __LINE__,  __func__, buf, tmp);
}

static void _btree_inorder_walk(FILE *fp, btree_node_p node, char parent_key[64])
{
	if (!node) {
		return;
	}

	char buf[64]; memset(buf, '\0', sizeof(buf)-1);
	_btree_print_key(node, buf);

	if (strlen(parent_key)) {
//fprintf(stderr, "%s [%d]: %s() - AAA - has parent\n",
//	__FILE__, __LINE__,  __func__);
		fprintf(fp, "%s -- %s;\n", parent_key, buf);
	} else {
//fprintf(stderr, "%s [%d]: %s() - BBB - is parent\n",
//	__FILE__, __LINE__,  __func__);
		fprintf(fp, "%s;\n", buf);
	}

	for (int idx = 0; idx < node->nkeys; idx++) {
		_btree_inorder_walk(fp, node->child[idx], buf);
	}
}
#endif

void btree_print_dot(const char* filename, btree_node_p node)
{
#ifdef DEBUG
	if (!filename) return;
	if (!node) return;
	FILE *fp = fopen(filename, "w");
	fprintf(fp, "graph %s\n", "tree");
	fprintf(fp, "{\n");

	char buf[64]; memset(buf, '\0', sizeof(buf)-1);
	_btree_print_key(node, buf);

	_btree_inorder_walk(fp, node, "");

	fprintf(fp, "}\n");
	fclose(fp);
#endif
}



// b-tree private

static btree_node_p root = NULL;

static btree_node_p _btree_allocate_node()
{
	btree_node_p ptr = NULL;
	ptr = malloc(sizeof(*ptr));
	if (!ptr) {
		btree_failure("%s [%d]: %s() - allocation failed",
			      __FILE__, __LINE__, __func__);
	}
	for (int idx = 0; idx < TSIZE; idx++) {
		ptr->key[idx] = NULL;
	}

	ptr->child = malloc(2 * TSIZE * sizeof(*ptr->child));
	if (!ptr->child) {
		btree_failure("%s [%d]: %s() - allocation failed",
			      __FILE__, __LINE__, __func__);
	}

	for (int idx = 0; idx < (2 * TSIZE); idx++) {
		ptr->child[idx] = NULL;
	}
	ptr->is_leaf = true;

	return ptr;
}

static void _btree_destroy_node(btree_node_p* node)
{
	if (!node) {
		return;
	}

	for (int idx = (*node)->nkeys - 1; idx >= 0; idx--) {
		_btree_destroy_node(&(*node)->child[idx]);
	}
	free((*node)->child);

	free(*node);
	*node = NULL;
}

static void _btree_write(btree_node_p node)
{
	// mock / dummy - use queue to "serialize" tree structure
	btree_debug("...write\n");
}

static void _btree_read(btree_node_p node)
{
	// mock / dummy - use queue to "de-serialize" queue into b-tree
	btree_debug("...write\n");
}


// b-tree

btree_node_p btree_root()
{
	return root;
}

element_p create_key(int *data, int key)
{
	element_p elem = NULL;

	elem = malloc(sizeof(*elem));
	if (!elem) {
		btree_failure("%s [%d]: %s() - allocation failed",
			      __FILE__, __LINE__, __func__);
	}
	elem->data = data;
	elem->val = key;

	return elem;
}

void* destroy_key(element_p *key)
{
	if (!key) {
		return NULL;
	}
	void* ptr = NULL;
	ptr = (*key)->data;
	free(*key);
	*key = NULL;

	return ptr;
}


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
btree_node_p btree_search(btree_node_p node, element_p key, int* idx_result)
{
	int idx;
	if (!idx_result) {
		btree_failure("%s [%d]: %s() - called with NULL argument",
			      __FILE__, __LINE__, __func__);
	}

	idx = 1;
	while (idx <= node->nkeys && key->val > node->key[idx]->val) {
		idx++;
	}
	if (idx <= node->nkeys && key->val == node->key[idx]->val) {
		// found, return node (and idx)
		*idx_result = idx;
		return node;
	} else if (node->is_leaf) {
		return NULL;
	} else {
		_btree_read(node->child[idx]);
		return btree_search(node->child[idx], key, idx_result);
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
	node->is_leaf = true;
	node->nkeys = 0;
	_btree_write(node);
	root = node;
}

void btree_destroy(btree_node_p node)
{
	if (!node) {
		return;
	}
	// TODO destroy element types       
	for (int idx = node->nkeys; idx > 0; idx--) {
		btree_destroy(node);
	}
	if (node == root) {
		_btree_destroy_node(&node);
		root = NULL;
	} else {
		_btree_destroy_node(&node);
	}
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
	btree_node_p y = node->child[idx];
	z->is_leaf = y->is_leaf;
	z->nkeys = TSIZE - 1;
	for (int jdx = 1; jdx < TSIZE - 1; jdx++) { // TODO jdx = 0    
		z->key[jdx] = y->key[jdx + TSIZE];
	}
	if (!y->is_leaf) {
		for (int jdx = 1; jdx < TSIZE; jdx++) {
			z->child[jdx] = y->child[jdx + TSIZE];
		}
	}
	y->nkeys = TSIZE - 1;
	for (int jdx = node->nkeys + 1; jdx >= idx + 1; jdx--) {
		node->child[jdx + 1] = node->child[jdx];
	}
	node->child[idx + 1] = z;
	for (int jdx = node->nkeys; jdx >= idx; jdx--) {
		node->key[jdx + 1] = node->key[jdx];
	}
	node->key[idx] = node->key[TSIZE]; // TODO TSIZE-1
	node->nkeys = node->nkeys + 1;
	_btree_write(y);
	_btree_write(z);
	_btree_write(node);
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
void btree_insert(element_p key)
{
/*
	btree_node_p root_bk = root;
// TODO check if root is NULL
	if (root_bk->nkeys == 2 * TSIZE - 1) {
		// root node is full, split and insert into new root
		btree_node_p root_new = _btree_allocate_node();
		root = root_new;
		root_new->is_leaf = false;
		root_new->nkeys = 0;
		root_new->child[0] = root_bk;
		btree_split_child(root_new, 1);
		btree_insert_nonfull(root_new, key);
	} else {
		// root node is not full
		btree_insert_nonfull(root_bk, key);
	}
/*/
	if (NULL == root) { // root is not
		root = _btree_allocate_node();
		root->key[0] = key;
		root->nkeys = 1;
	} else {
		if (root->nkeys >= 2 * TSIZE -1) { // root is full
			btree_node_p root_new = _btree_allocate_node();
			root_new->child[0] = root;
			btree_split_child(root_new, 1); // TODO use key instead  

			int idx = 0;
			if (root_new->key[0] < key) {
				idx++;
			}
			btree_insert_nonfull(root_new->child[idx], key);

			root = root_new;
		} else {
			// root node is not full
			btree_insert_nonfull(root, key);
		}
	}
// */
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
void btree_insert_nonfull(btree_node_p node, element_p key)
{
	if (!node) {
		btree_failure("%s [%d]: %s() - node was passed NULL",
			      __FILE__, __LINE__, __func__);
	}
	if (!key) {
		btree_failure("%s [%d]: %s() - key was passed NULL",
			      __FILE__, __LINE__, __func__);
	}

//	int idx = node->nkeys; // TODO rm
	int idx = node->nkeys - 1;
	if (node->is_leaf) {
//		while (idx >= 1 && key->val < node->key[idx]->val) {
		while (idx >= 0 && key->val < node->key[idx]->val) {
			node->key[idx + 1] = node->key[idx];
			idx--;
		}
// TODO what to do, if idx < 0?
		node->key[idx + 1] = key;
// TODO is it really 'idx + 1' or just 'idx'?
		node->nkeys++;
		_btree_write(node);
	} else {
//		while (idx >= 1 && key->val < node->key[idx]->val) {
		while (idx >= 0 && key->val < node->key[idx]->val) {
			idx--;
		}
// TODO what to do, if idx < 0?
		idx++;
		_btree_read(node->child[idx]); // TODO why? here??
		if (node->child[idx]->nkeys == 2 * TSIZE - 1) {
			btree_split_child(node, idx);
			if (key->val > node->key[idx]->val) {
				idx++;
			}
		}
		btree_insert_nonfull(node->child[idx], key);
	}
}
