/*
  implementation
 */

#include "binary-search-tree.h"

#include <stdint.h>


/* private */

static node_p _tree_root;


static void _tree_failure(const char* msg)
{
	perror(msg);
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
static void _tree_inorder_walk(FILE *fp, node_p node, const char* label)
{
	if (node) {
		_tree_inorder_walk(fp, node->left, "L");
		if (node->parent) {
			fprintf(fp, "%lld -- %lld [ label=\"%s\" ];\n",
				node->parent->key, node->key, label);
		} else {
			fprintf(fp, "%lld;\n", node->key);
		}
		_tree_inorder_walk(fp, node->right, "R");
	}
}
#endif

/* public */

void tree_print_dot(const char* filename, node_p node)
{
#ifdef DEBUG
	if (!filename) return;
	if (!node) return;
	FILE *fp = fopen(filename, "w");
	fprintf(fp, "graph %s\n", "tree");
	fprintf(fp, "{\n");
	if (node->left) _tree_inorder_walk(fp, node->left, "L");
	if (node->right) _tree_inorder_walk(fp, node->right, "R");
	if (!node->left && !node->right) fprintf(fp, "%lld;\n", node->key);
	fprintf(fp, "}\n");
	fclose(fp);
#endif
}

node_p tree_root(void)
{
	return _tree_root;
}

void* tree_get_data(node_p node)
{
	return node->data;
}

/*
  TREE-SEARCH

  if x == NIL or k == x.key
    return x
  if k < x.key
    return TREE-SEARCH(x.left, k)
  else
    return TREE-SEARCH(x.right, k)
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
    else
      x = x.right
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
node_p tree_minimum(node_p node)
{
	node_p ptr = node;
	while (NULL != (ptr = node->left)) {
		node = ptr;
	}
	return node;
}

/*
  TREE-MAXIMUM(x)

  while x.right != NIL
    x = x.right
  return x
 */
node_p tree_maximum(node_p node)
{
	node_p ptr = node;
	while (NULL != (ptr = node->right)) {
		node = ptr;
	}
	return node;
}

/*
  TREE-SUCCESSOR(x)

  if x.right != NIL
    return TREE-MINIMUM(x.right)
  x = x.p
  while y != NIL and x == y.right
    x = y
    y = y.p
  return y
 */
node_p tree_successor(node_p node)
{
	if (node->right) return tree_minimum(node->right);

	node_p parent = node->parent;
	while ((parent != NULL) && (node == parent->right)) {
		node = parent;
		parent = parent->parent;
	}
	return parent;
}

/*
  TREE-PREDECESSOR(x)

  if x.left != NIL
    return TREE-MAXIMUM(x.left)
  x = x.p
  while y != NIL and x == y.left
    x = y
    y = y.p
  return y
 */
node_p tree_predecessor(node_p node)
{
	if (node->left) return tree_maximum(node->left);

	node_p parent = node->parent;
	while ((parent != NULL) && (node == parent->left)) {
		node = parent;
		parent = parent->parent;
	}
	return parent;
}


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
	node_p parent = NULL;
	node_p tmp = tree_root();
	while (tmp) {
		parent = tmp;
		if (key < tmp->key) {
			tmp = tmp->left;
		} else {
			tmp = tmp->right;
		}
	}
	node_p node = malloc(sizeof(*node));
	if (!node) _tree_failure("allocation failed");
	node->key = key;
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	node->parent = parent;
	if (!parent) {
		_tree_root = node; // tree was empty
	} else if (node->key < parent->key) {
		parent->left = node;
	} else {
		parent->right = node;
	}
}

/*
  TRANSPLANT(T, u, v)

  if u.p == NIL
    T.root = v
  elseif u == u.p.left
    u.p.left = v
  else
    u.p.right = v
  if v != NIL
    v.p = u.p
 */
static void _tree_transplant(node_p node, node_p new_subtree)
{
	// replace (down)
	if (!node->parent) {
		_tree_root = new_subtree;
	} else if (node == node->parent->left) {
		node->parent->left = new_subtree;
	} else {
		node->parent->right = new_subtree;
	}

	// set new parent (up)
	if (new_subtree) {
		new_subtree->parent = node->parent;
	}
}
void tree_transplant(node_p node, node_p new_subtree)
{
	_tree_transplant(node, new_subtree);
}

/*
  TREE-DELETE(T, z)

  if z->left == NIL
    TRANSPLANT(T, z, z.right)
  elseif z.right == NIL
    TRANSPLANT(T, z, z.left)
  else
    y = TREE-MINIMUM(z.right)
    if y.p != z
      TRANSPLANT(T, y, y.right)
      y.right = z.right
      y.right.p = y
    TRANSPLANT(T, z, y)
    y.left = z.left
    y.left.p = y
 */
void* tree_delete(node_p *deletee)
{
	if (NULL == (*deletee)) return NULL;
	node_p node = *deletee;
	if (!node->left) {
		// left of destination node is NULL
		_tree_transplant(node, node->right);
	} else if (!node->right) {
		// right of destination node is NULL
		_tree_transplant(node, node->left);
	} else {
		// destination node has branches to reconnect
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			_tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		_tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	void* ptr = node->data;
	if (!node->parent) _tree_root = NULL;
	free(*deletee); *deletee = NULL;
	return ptr;
}


