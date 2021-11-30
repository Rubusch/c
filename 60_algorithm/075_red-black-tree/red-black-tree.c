/*
  implementation
 */

#include "red-black-tree.h"

#include <stdint.h>

/* private */

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
		fprintf(fp, "%lld [style=filled, %s];\n",
			node->key,
			(node->color == RED) ? "fillcolor=red"
			: "fontcolor=white, fillcolor=black");
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

void tree_insert(uint64_t key, void* data)
{
	node_p parent = NULL;
	node_p tmp = tree_root();
	while (tmp != NULL) {
		parent = tmp;
		if (key < tmp->key)
			tmp = tmp->left;
		else
			tmp = tmp->right;
	}
	node_p node = malloc(sizeof(*node));
	if (!node) _tree_failure("allocation failed");
	node->parent = parent;
	node->key = key;
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	if (!parent) {
		_root = node; // tree was empty
	} else if (node->key < parent->key) {
		parent->left = node;
	} else {
		parent->right = node;
	}
}
// */

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

static void _tree_transplant(node_p node, node_p new_subtree)
{
	// replace (down)
	if (!node->parent) {
		_root = new_subtree;
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
// */

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

void* tree_delete(node_p node)
{
	if (!node) return NULL;
	if (!node->left) {
		// left of destination location is NULL
		_tree_transplant(node, node->right);
	} else if (!node->right) {
		// right of destination location is NULL
		_tree_transplant(node, node->left);
	} else {
		// destination location has branches to reconnect
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
	free(node);
	return ptr;
}
// */

/*
  LEFT-ROTATE

  y = x.right
  x.right = y.left
  if y.left != T.nil
    y.left.p = x
  y.p = x.p
  if x.p == T.nil
    T.root = y
  elseif x == x.p.left
    x.p.left = y
  else
    x.p.right = y
  y.left = x
  x.p = y
 */
void red_black_left_rotate(node_p node)
{
	node_p tmp = node->right;
	node->right = tmp->left;
	if (tmp->left != NULL) {
		tmp->left->parent = node;
	}
	tmp->parent = node->parent;
	if (node->parent == NULL) {
		_tree_root = tmp;
	} else if (node == node->parent->left) {
		node->parent->left = tmp;
	} else {
		node->parent->right = tmp;
	}
	tmp->left = node;
	node->parent = tmp;
}

/*
  RIGHT-ROTATE

  y = x.left
  x.left = y.right
  if y.right != T.nil
    y.right.p = x
  y.p = x.p
  if x.p == T.nil
    T.root = y
  elseif x == x.p.right
    x.p.right = y
  else
    x.p.left = y
  y.right = x
  x.p = y
 */
void red_black_right_rotate(node_p node)
{
	node_p tmp = node->left;
	node->left = tmp->right;
	if (tmp->right != NULL) {
		tmp->right->parent = node;
	}
	tmp->parent = node->parent;
	if (node->parent == NULL) {
		_tree_root = tmp;
	} else if (node == node->parent->right) {
		node->parent->right = tmp;
	} else {
		node->parent->left = tmp;
	}
	tmp->right = node;
	node->parent = tmp;
}

/*
  RB-INSERT-FIXUP(T, z)

  while z.p.color == RED
    if z.p == z.p.p.left
      y = z.p.p.right
      if y.color == RED
        z.p.color = BLACK
	y.color = BLACK
	z.p.p.color = RED
	z = z.p.p
      else
        if z == z.p.right
	  z = z.p
	  LEFT-ROTATE(T, z)
	z.p.color = BLACK
	z.p.p.color = RED
	RIGHT-ROTATE(T, z.p.p)
    else
      <same as then clause with "right" and "left" exchanged>
  T.root.color = BLACK
*/
static void _red_black_insert_fixup(node_p node)
{
	if (!node->parent) {
		node->color = BLACK;
		return;
	}

	while (node->parent->color == RED) {
		if (node->parent == node->parent->parent->left) {
			node_p node_y = node->parent->parent->right;
			if (node_y->color == RED) {
				node->parent->color = BLACK;
				node_y->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					red_black_left_rotate(node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				red_black_right_rotate(node->parent->parent);
			}
		} else {
			node_p node_y = node->parent->parent->left;
			if (node_y->color == RED) {
				node->parent->color = BLACK;
				node_y->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					red_black_right_rotate(node); // TODO also rotation exchanged?
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				red_black_left_rotate(node->parent->parent); // also rotation exchanged?
			}
		}
	}
	_tree_root->color = BLACK;
}
void red_black_insert_fixup(node_p node)
{
	_red_black_insert_fixup(node);
}

/*
  RB-INSERT(T, z)

  y = T.nil
  x = T.root
  while x != T.nil
    y = x
    if z.key < x.key
      x = x.left
    else
      x = x.right
  z.p = y
  if y == T.nil
    T.root = z
  elseif z.key < y.key
    y.left = z
  else
    y.right = z
  z.left = T.nil
  z.right = T.nil
OA  z.color = RED
  RB-INSERT-FIXUP(T, z)
*/
void red_black_insert(uint64_t key, void* data)
{
	node_p node = malloc(sizeof(*node));
	if (!node) _tree_failure("allocation failed");
	node->color = RED;
	node->key = key;
	node->data = data;
	node->left = NULL;
	node->right = NULL;

	node_p node_y = NULL;
	node_p node_x = tree_root();
	while (node_x) {
		node_y = node_x;
		if (node->key < node_x->key) {
			node_x = node_x->left;
		} else {
			node_x = node_x->right;
		}
	}
	node->parent = node_y;
	if (!node_y) {
		_tree_root = node;
	} else if (node->key < node_y->key) {
		node_y->left = node;
	} else {
		node_y->right = node;
	}
	node->left = NULL;
	node->right = NULL;
	node->color = RED;
	_red_black_insert_fixup(node);
}

/*
  RB-DELETE-FIXUP(T, x)

  while x != T.root and x.color == BLACK
    if x == x.p.left
      w = x.p.right
      if w.color == RED
        w.color = BLACK
	x.p.color = RED
	LEFT-ROTATE(T, x.p)
	w = x.p.right
      if w.left.color == BLACK and w.right.color == BLACK
        w.color = RED
	x = x.p
      else
        if w.right.color == BLACK
	  w.left.color = BLACK
	  w.color = RED
	  RIGHT-ROTATE(T, w)
	  w = x.p.right
	w.color = x.p.color
	x.p.color = BLACK
	w.right.color = BLACK
	LEFT-ROTATE(T, x.p)
	x = T.root
    else
      <same as then clause with "right" and "left" exchanged>
  x.color = BLACK
*/
static void _red_black_delete_fixup(node_p node)
{
	while ((node != tree_root()) && (node->color == BLACK)) {
		if (node == node->parent->left) {
			node_p node_w = node->parent->right;
			if (node_w->color == RED) {
				node_w->color = BLACK;
				node->parent->color = RED;
				red_black_left_rotate(node->parent);
				node_w = node->parent->right;
			}
			if ((node_w->left->color == BLACK) && (node_w->right->color == BLACK)) {
				node_w->color = RED;
				node = node->parent;
			} else {
				if (node_w->right->color == BLACK) {
					node_w->left->color = BLACK;
					node_w->color = RED;
					red_black_right_rotate(node_w);
					node_w = node->parent->right;
				}
				node_w->color = node->parent->color;
				node->parent->color = BLACK;
				node_w->right->color = BLACK;
				red_black_left_rotate(node->parent);
				node = tree_root();
			}
		} else {
			node_p node_w = node->parent->left;
			if (node_w->color == RED) {
				node_w->color = BLACK;
				node->parent->color = RED;
				red_black_right_rotate(node->parent);
				node_w = node->parent->left;
			}
			if ((node_w->right->color == BLACK) && (node_w->left->color == BLACK)) {
				node_w->color = RED;
				node = node->parent;
			} else {
				if (node_w->left->color == BLACK) {
					node_w->right->color = BLACK;
					node_w->color = RED;
					red_black_left_rotate(node_w);
					node_w = node->parent->left;
				}
				node_w->color = node->parent->color;
				node->parent->color = BLACK;
				node_w->left->color = BLACK;
				red_black_right_rotate(node->parent);
				node = tree_root();
			}
		}
	}
	node->color = BLACK;
}
void red_black_delete_fixup(node_p node)
{
	_red_black_delete_fixup(node);
}

/*
  RB-TRANSPLANT(T, u, v)

  if u.p == T.nil
    T.root = v
  elseif u == u.p.left
    u.p.left = v
  else
    u.p.right = v
  v.p = u.p
 */
static void _red_black_transplant(node_p node, node_p new_subtree)
{
	if (!node->parent) {
		_tree_root = new_subtree;
	} else if (node == node->parent->left) {
		node->parent->left = new_subtree;
	} else {
		node->parent->right = new_subtree;
	}

	new_subtree->parent = node->parent;
}
void red_black_transplant(node_p node, node_p new_subtree)
{
	_red_black_transplant(node, new_subtree);
}

/*
  RB-DELETE(T, z)

  y = z
  y-original-color = y.color
  if z.left == T.nil
    x = z.right
    RB-TRANSPLANT(T, z, z.right)
  elseif z.right == T.nil
    x = z.left
    RB-TRANSPLANT(T, z, z.left)
  else
    y = TREE-MINIMUM(z.right)
    y-original-color = y.color
    x = y.right
    if y.p == z
      x.p = y
    else
      RB-TRANSPLANT(T, y, y.right)
      y.right = z.right
      y.right.p = y
    RB-TRANSPLANT(T, z, y)
    y.left = z.left
    y.left.p = y
    y.color = z.color
  if y-original-color == BLACK
    RB-DELETE-FIXUP(T, x)
*/
void* red_black_delete(node_p node)
{
	node_p node_x = NULL;
	node_p tmp = node;
	color_t orig_color = tmp->color;
	if (!node->left) {
		node_x = node->right;
		_red_black_transplant(node, node->right);
	} else if (!node->right) {
		node_x = node->left;
		_red_black_transplant(node, node->left);
	} else {
		tmp = tree_minimum(node->right);
		orig_color = tmp->color;
		node_x = tmp->right;
		if (tmp->parent == node) {
			node_x->parent = tmp;
		} else {
			_red_black_transplant(node, tmp);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		_red_black_transplant(node, tmp);
		tmp->left = node->left;
		tmp->left->parent = tmp;
		tmp->color = node->color;
	}
	if (orig_color == BLACK) {
		_red_black_delete_fixup(node_x);
	}

// TODO return data content
	return NULL;
}

