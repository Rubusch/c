/*
  implementation
 */

#include "red-black-tree.h"

#include <stdint.h>
#include <stdbool.h>


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
	if (!node) {
		return;
	}

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
	fprintf(fp, "%lld [style=filled, %s];\n",
		node->key,
		(node->color == RED)
		? "fillcolor=red"
		: "fontcolor=white, fillcolor=black");
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


  visual: LEFT-ROTATE

      (parent)           (parent)
         /                  /
       node               tmp
       /  \     ==>       /
  (some)  tmp           node
          /             /  \
        child       (some) child
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


  visual

      (parent)        (parent)
         /               /
       node            tmp
       /  \     ==>      \
     tmp (some)         node
       \                 / \
      child          child (some)

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
	while ((node->parent != NULL) && (node->parent->color == RED)) {
		if (node->parent == node->parent->parent->left) {
			node_p node_y = node->parent->parent->right;
			if ((node_y != NULL) && (node_y->color == RED)) {
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
			if ((node_y != NULL) && (node_y->color == RED)) {
				node->parent->color = BLACK;
				node_y->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					red_black_right_rotate(node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				red_black_left_rotate(node->parent->parent);
			}
		}
	} /* while */
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
  z.color = RED
  RB-INSERT-FIXUP(T, z)
*/
void red_black_insert(uint64_t key, void* data)
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
static void _red_black_delete_fixup(node_p node, node_p parent)
{
	if (!parent) return; // node may be NULL, parent may not be!
	// considering the case is worthless, if not even parent is
	// known

	node_p sibling;

        // NB: node can be NULL
	color_t node_color = (node == NULL) ? BLACK : node->color;
	while (node != tree_root() && (node_color == BLACK || node == NULL)) {
		if (node == parent->left) { // Q: What if
					    // (parent->left == NULL
					    // && node is NULL)?
			                    // A: Nvm, treat as
					    // parent->left or
					    // parent-right makes no
					    // difference
			// is left child
			sibling = parent->right; // can be NULL
			bool is_sibling_red = (sibling == NULL) ? false : (sibling->color == RED);
			if (is_sibling_red) {
/*
  case 1: sibling is RED

            |                                 |
    parent (B)                               (D)
          /   \RED               new     RED/   \
  node (A)     (D) sibling  ==>  sibling (B)     (E)
       / \     / \                       / \     / \
             (C) (E)              node (A) (C)
             / \ / \                   / \ / \

  Case 1 occurs when the sibling ['w' in the literature] of node [x]
  is RED. Since sibling must have black children, we can switch the
  colors of sibling and node->parent and then perform a left-rotation
  on node->parent without violating any of the red-black
  properties. The new sibling of node, which is oneof sibling's
  children prior to the rotation, is now BLACK, and thus we have
  converted case 1 into case 2, 3 or 4.
*/
				sibling->color = BLACK;
				parent->color = RED;
				red_black_left_rotate(parent);
				sibling = parent->right;
			}

			if (sibling == NULL) {
				/* additional case: due to sibling being NIL, just move up */
				node = parent;
				parent = node->parent;

			} else {
				bool is_sibling_left_black =
					(sibling->left == NULL) ?
					true : (sibling->left->color == BLACK);

				bool is_sibling_right_black =
					(sibling->right == NULL) ?
					true : (sibling->right->color == BLACK);

				if (is_sibling_left_black
				    && is_sibling_right_black) {
/*
  case 2: sibling is BLACK, and both of sibling's children are BLACK

           |                              |
   parent (B)                   new node (B)
         /   \                          /   \RED
  node (A)     (D) sibling  ==>       (A)     (D)
       / \     / \                    / \     / \
             (C) (E)                        (C) (E)
             / \ / \                        / \ / \

  In case 2 both of sibling's children are BLACK. Since sibling is
  also BLACK we take one BLACK off both node and sibling leaving node
  with only one BLACK and leaving sibling RED. To compensate for
  removing one BLACK from node an dsibling, we would like to add an
  extra BLACK to node->parent, which was originally either RED or
  BLACK. We do so by repeating the while loop with node->parent as the
  new node. Observe that if we enter case 2 through case 1, the new
  node is RED-and-BLACK, since the original node->parent was
  RED. Hence, the color value attribute of the new node is RED, and
  the loop terminates when it tests the loop condition. We then color
  the new node (singly) BLACK.
*/
					sibling->color = RED;
					node = parent;
					parent = node->parent;
				} else {
					if ((sibling->left != NULL)
					    && is_sibling_right_black) {
/*
  case 3: sibling is BLACK, sibling's left child is RED, and sibling's
  right child is BLACK

            |                         |
    parent (B)                       (B)
          /   \                     /   \
  node (A)     (D) sibling  ==>  (A)     (C) new sibling
       / \  RED/ \               / \     / \RED
             (C) (E)                       (D)
             / \ / \                       / \
                                             (E)
                                             / \

  Case 3 occurs when sibling is BLACK, its left child is RED, and its
  right child is BLACK. We can switch the colors of sibling and its
  left child sibling->left an dthen perform a right rotation on
  sibling without violating any of the red-black properties. The new
  sibling of node is now a BLACK node with a RED right child, and thus
  we have transformed case 3 and case 4.
*/
						sibling->left->color = BLACK;
						sibling->color = RED;
						red_black_right_rotate(sibling);
						sibling = parent->right;
					}
/*
  case 4: sibling is BLACK and sibling's right child is RED

            |                            |
    parent (B)                          (D)
          /   \                        /   \
  node (A)     (D) sibling  ==>     (B)     (E)
       / \     / \RED               / \     / \
             (C) (E)              (A) (C)
             / \ / \              / \ / \

  Case 4 occurs when node's sibling is BLACK and sibling's right child
  is RED. By making some color changes and performing a left rotation
  on node->parent, we can remove the extra BLACK on node, making it
  singly BLACK, without violating any of the red-black
  properties. Setting x to be the root causes the while loop to
  terminate when it tests the loop condition.
*/
					sibling->color = parent->color;
					parent->color = BLACK;
					if (sibling->right)
						sibling->right->color = BLACK;
					red_black_left_rotate(parent);
					node = tree_root();
					parent = node->parent; // NULL
				}
			}
		} else {
			// is right child
			sibling = parent->left;
			bool is_sibling_red = (sibling == NULL) ?
				false : (sibling->color == RED);

			if (is_sibling_red) {
				/* case 1 */
				sibling->color = BLACK;
				parent->color = RED;
				red_black_right_rotate(parent);
				sibling = parent->left;
			}

			if (sibling == NULL) {
				/* additional case */
				node = parent;
				parent = node->parent;
			} else {
				bool is_sibling_left_black =
					(sibling->left == NULL) ?
					true : (sibling->left->color == BLACK);

				bool is_sibling_right_black =
					(sibling->right == NULL) ?
					true : (sibling->right->color == BLACK);

				if (is_sibling_left_black
				    && is_sibling_right_black) {
					/* case 2 */
					sibling->color = RED;
					node = parent;
					parent = node->parent;
				} else {
					if ((sibling->right == NULL)
					    && is_sibling_left_black) {
						/* case 3 */
						sibling->right->color = BLACK;
						sibling->color = RED;
						red_black_left_rotate(sibling);
						sibling = parent->left;
					}
					/* case 4 */
					sibling->color = parent->color;
					parent->color = BLACK;
					if (sibling->left)
						sibling->left->color = BLACK;
					red_black_right_rotate(parent);
					node = tree_root();
					parent = node->parent; // NULL
				}
			}
		}
		node_color = (node == NULL) ? BLACK : node->color;
	}
	if (node) {
		node->color = BLACK;
	}
}
void red_black_delete_fixup(node_p node, node_p parent)
{
	_red_black_delete_fixup(node, parent);
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
void* red_black_delete(node_p *deletee)
{
	node_p node = *deletee;
	node_p parent = NULL;
	node_p child = NULL;
	color_t orig_color = node->color;
	if (!node) return NULL;
	if (!node->left) {
		// left of destination node is NULL
		child = node->right;
		parent = node->parent; // child will replace "node"
		_red_black_transplant(node, node->right);
	} else if (!node->right) {
		// right of destination node is NULL
		child = node->left;
		parent = node->parent;
		_red_black_transplant(node, node->left);
	} else {
		// destination node has branches to reconnect
		node_p tmp = tree_minimum(node->right);
		orig_color = tmp->color;
		child = tmp->right;
		parent = tmp; // added for passing NULL to the fixup
		if (tmp->parent != node) {
			_red_black_transplant(tmp, tmp->right);
			parent = tmp->parent; // added to adjust
					      // parent after
					      // transplant
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		_red_black_transplant(node, tmp);
/*		parent = node->parent; // no further parent adjustment here */
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
		tmp->color = node->color;
	}
	if (orig_color == BLACK && parent != NULL) {
		_red_black_delete_fixup(child, parent);
	}

	void *ptr = node->data;
	free(node); /* free allocated memory, in case location is not
		     * identical with *deletee, anymore - *deletee is
		     * only used here to be assigned to NULL */
	*deletee = NULL;
	return ptr;
}
