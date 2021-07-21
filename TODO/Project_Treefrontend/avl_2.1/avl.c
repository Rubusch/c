/*=+cch======================================================================*/
/*  NAME     : avl.c                                                         */
/*  FUNCTION :                                                               */
/*---------------------------------------------------------------------------*/
/*  (C) Copyright 2008 Nokia Siemens Networks                                */
/*---------------------------------------------------------------------------*/
/*  AUTHOR   : demxjh46                                                      */
/*  CREATED  : 2008-02-05                                                    */
/*======================================================================cch+=*/
/* Produced by texiweb from libavl.w on 2001/06/22 at 10:29. */

/* libavl - library for manipulation of binary trees.
   Copyright (C) 1998, 1999, 2000 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.

   The author may be contacted at <blp@gnu.org> on the Internet, or
   as Ben Pfaff, 12167 Airport Rd, DeWitt MI 48820, USA through more
   mundane means.
*/
#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/string.h>
#if 0
#include <linux/malloc.h>
#else
#include <linux/slab.h>
#endif
#define assert(arg)
#endif

#ifndef __KERNEL__
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#endif
#include <avl.h>

#ifndef avl_allocate
/* Allocates |size| bytes of space using |malloc()|.  Aborts if out of
   memory. */
void *avl_allocate(size_t size)
{
  void *vp;

  if (size == 0)
    return NULL;

#ifdef __KERNEL__
  vp = kmalloc(size, GFP_ATOMIC);
#else
  vp = malloc(size);
#endif
  if (vp == NULL) {
#ifdef __KERNEL__
    printk(KERN_ERR "out of memory\r\n");
#else
    fprintf(stderr, "out of memory\n");
    exit(EXIT_FAILURE);
#endif
  }
  return vp;
}
#endif /* |avl_allocate()| */

#ifndef avl_free
#ifdef __KERNEL__
#define avl_free(BLOCK) kfree(BLOCK)
#else
#define avl_free(BLOCK) free(BLOCK)

void(avl_free)(void *p) { free(p); }
#endif
#endif /* |avl_free()| */

/* Creates and returns a new AVL tree.
   Returns |NULL| if memory allocation failed. */
struct avl_tree *avl_create(avl_comparison_func *compare, void *param)
{
  struct avl_tree *tree;

  assert(compare != NULL);
  tree = avl_allocate(sizeof *tree);
  if (tree == NULL)
    return NULL;

  tree->avl_root = NULL;
  tree->avl_compare = compare;
  tree->avl_param = param;
  tree->avl_count = 0;

  return tree;
}

/* Search |tree| for an item matching |item|, and return it if found.
   Otherwise return |NULL|. */
void *avl_find(const struct avl_tree *tree, const void *item, int flag)
{
  const struct avl_node *p;
  const struct avl_node *ref = NULL;

  assert(tree != NULL && item != NULL);
  for (p = tree->avl_root; p != NULL;) {
    int diff = tree->avl_compare(item, p->avl_data, tree->avl_param);

    if (diff < 0) {
      if (flag == AVL_GE) {
        if (ref == NULL)
          ref = p;
        else {
          diff = tree->avl_compare(p->avl_data, ref->avl_data, tree->avl_param);
          if (diff < 0)
            ref = p;
        }
      }
      p = p->avl_link[0];
    } else if (diff > 0) {
      if (flag == AVL_LE) {
        if (ref == NULL)
          ref = p;
        else {
          diff = tree->avl_compare(p->avl_data, ref->avl_data, tree->avl_param);
          if (diff > 0)
            ref = p;
        }
      }
      p = p->avl_link[1];
    } else {
      return p->avl_data;
    }
  }

  if (flag == AVL_LE || flag == AVL_GE) {
    if (ref)
      return ref->avl_data;
  }
  return NULL;
}

/* KGB */
void *avl_find_and_traverser_init(const struct avl_tree *tree, const void *item,
                                  int flag, struct avl_traverser *trav)
{
  const struct avl_node *p;
  void *avl_data;

  assert(tree != NULL && item != NULL);
  avl_data = avl_find(tree, item, flag);

  if (!avl_data)
    return NULL; /* no exact match */
  if (trav) {
    trav->avl_height = 0;
    trav->avl_node = NULL;
  }

  for (p = tree->avl_root; p != NULL;) {
    int diff = tree->avl_compare(avl_data, p->avl_data, tree->avl_param);

    if (diff < 0) {
      if (trav)
        trav->avl_stack[trav->avl_height++] = ( struct avl_node * )p;
      p = p->avl_link[0];
    } else if (diff > 0) {
      p = p->avl_link[1];
    } else {
      trav->avl_node = p->avl_link[1];
      return p->avl_data;
    }
  }

  return NULL;
}

/* Inserts |item| into |tree| and returns a pointer to |item|'s address.
   If a duplicate item is found in the tree,
   returns a pointer to the duplicate without inserting |item|.
   Returns |NULL| in case of memory allocation failure. */
void **avl_probe(struct avl_tree *tree, void *item)
{
  struct avl_node *t;     /* Node below which rebalancing occurs. */
  struct avl_node *s;     /* Node to which rebalancing is applied. */
  struct avl_node *p;     /* Traverses tree; root of rebalancing. */
  struct avl_node *q, *r; /* Scratch pointers. */

  assert(tree != NULL && item != NULL);

  t = ( struct avl_node * )&tree->avl_root;
  s = p = t->avl_link[0];

  if (s == NULL) {
    tree->avl_count++;
    assert(tree->avl_count == 1);
    q = t->avl_link[0] = avl_allocate(sizeof *q);
    if (q == NULL)
      return NULL;

    q->avl_data = item;
    q->avl_link[0] = q->avl_link[1] = NULL;
    q->avl_bal = 0;
    return &q->avl_data;
  }

  for (;;) {
    int diff = tree->avl_compare(item, p->avl_data, tree->avl_param);

    if (diff < 0) {
      p->avl_cache = 0;
      q = p->avl_link[0];
      if (q == NULL) {
        p->avl_link[0] = q = avl_allocate(sizeof *q);
        break;
      }
    } else if (diff > 0) {
      p->avl_cache = 1;
      q = p->avl_link[1];
      if (q == NULL) {
        p->avl_link[1] = q = avl_allocate(sizeof *q);
        break;
      }
    } else
      return &p->avl_data;

    if (q->avl_bal != 0) {
      t = p;
      s = q;
    }
    p = q;
  }

  tree->avl_count++;
  q->avl_data = item;
  q->avl_link[0] = q->avl_link[1] = NULL;
  q->avl_bal = 0;

  r = p = s->avl_link[s->avl_cache];
  while (p != q) {
    p->avl_bal = p->avl_cache * 2 - 1;
    p = p->avl_link[p->avl_cache];
  }

  if (s->avl_cache == 0) {
    if (s->avl_bal == 0) {
      s->avl_bal = -1;
      return &q->avl_data;
    } else if (s->avl_bal == +1) {
      s->avl_bal = 0;
      return &q->avl_data;
    }

    if (r->avl_bal == -1) {
      p = r;
      s->avl_link[0] = r->avl_link[1];
      r->avl_link[1] = s;
      s->avl_bal = r->avl_bal = 0;
    } else {
      assert(r->avl_bal == +1);
      p = r->avl_link[1];
      r->avl_link[1] = p->avl_link[0];
      p->avl_link[0] = r;
      s->avl_link[0] = p->avl_link[1];
      p->avl_link[1] = s;
      if (p->avl_bal == -1)
        s->avl_bal = +1, r->avl_bal = 0;
      else if (p->avl_bal == 0)
        s->avl_bal = r->avl_bal = 0;
      else /* |p->avl_bal == +1| */
        s->avl_bal = 0, r->avl_bal = -1;
      p->avl_bal = 0;
    }
  } else {
    if (s->avl_bal == 0) {
      s->avl_bal = +1;
      return &q->avl_data;
    } else if (s->avl_bal == -1) {
      s->avl_bal = 0;
      return &q->avl_data;
    }

    assert(s->avl_bal == +1);
    if (r->avl_bal == +1) {
      p = r;
      s->avl_link[1] = r->avl_link[0];
      r->avl_link[0] = s;
      s->avl_bal = r->avl_bal = 0;
    } else {
      assert(r->avl_bal == -1);
      p = r->avl_link[0];
      r->avl_link[0] = p->avl_link[1];
      p->avl_link[1] = r;
      s->avl_link[1] = p->avl_link[0];
      p->avl_link[0] = s;
      if (p->avl_bal == +1)
        s->avl_bal = -1, r->avl_bal = 0;
      else if (p->avl_bal == 0)
        s->avl_bal = r->avl_bal = 0;
      else /* |p->avl_bal == -1| */
        s->avl_bal = 0, r->avl_bal = +1;
      p->avl_bal = 0;
    }
  }
  t->avl_link[s != t->avl_link[0]] = p;

  return &q->avl_data;
}

/* Inserts |item| into |tree|.
   Returns |NULL| if |item| was successfully inserted
   or if a memory allocation error occurred.
   Otherwise, returns the duplicate item. */
void *avl_insert(struct avl_tree *tree, void *item)
{
  void **p = avl_probe(tree, item);
  return p == NULL || *p == item ? NULL : *p;
}

/* Inserts |item| into |tree|, replacing any duplicate item.
   Returns |NULL| if |item| was inserted without replacing a duplicate,
   or if a memory allocation error occurred.
   Otherwise, returns the item that was replaced. */
void *avl_replace(struct avl_tree *tree, void *item)
{
  void **p = avl_probe(tree, item);
  if (*p == item)
    return NULL;
  else {
    void *r = *p;
    *p = item;
    return r;
  }
}

/* Asserts that |avl_insert()| succeeds at inserting |item| into |tree|. */
void(avl_assert_insert)(struct avl_tree *tree, void *item)
{
  void *p = avl_insert(tree, item);
  assert(p == NULL);
}

/* Asserts that |avl_delete()| really removes |item| from |tree|,
   and returns the removed item. */
void *(avl_assert_delete)(struct avl_tree *tree, void *item)
{
  void *p = avl_delete(tree, item);
  assert(p != NULL);
  return p;
}

/* Deletes from |tree| and returns an item matching |item|.
   Returns a null pointer if no matching item found. */
void *avl_delete(struct avl_tree *tree, const void *item)
{
  /* Stack of nodes. */
  struct avl_node *pa[AVL_MAX_HEIGHT]; /* Nodes. */
  unsigned char a[AVL_MAX_HEIGHT];     /* |avl_link[]| indexes. */
  int k = 1;                           /* Stack pointer. */

  struct avl_node *p;  /* Traverses tree to find node to delete. */
  struct avl_node **q; /* Points to |p|. */

  assert(tree != NULL);

  a[0] = 0;
  pa[0] = ( struct avl_node * )&tree->avl_root;
  p = tree->avl_root;
  for (;;) {
    int diff;

    if (p == NULL)
      return NULL;

    diff = tree->avl_compare(item, p->avl_data, tree->avl_param);
    if (diff == 0)
      break;

    pa[k] = p;
    if (diff < 0) {
      p = p->avl_link[0];
      a[k] = 0;
    } else if (diff > 0) {
      p = p->avl_link[1];
      a[k] = 1;
    }
    k++;
  }

  tree->avl_count--;
  item = p->avl_data;

  q = &pa[k - 1]->avl_link[a[k - 1]];
  if (p->avl_link[1] == NULL)
    *q = p->avl_link[0];
  else {
    struct avl_node *r = p->avl_link[1];
    if (r->avl_link[0] == NULL) {
      r->avl_link[0] = p->avl_link[0];
      *q = r;
      r->avl_bal = p->avl_bal;
      a[k] = 1;
      pa[k++] = r;
    } else {
      struct avl_node *s;
      int l = k++;

      for (;;) {
        a[k] = 0;
        pa[k++] = r;
        s = r->avl_link[0];
        if (s->avl_link[0] == NULL)
          break;

        r = s;
      }

      a[l] = 1;
      pa[l] = s;
      s->avl_link[0] = p->avl_link[0];
      r->avl_link[0] = s->avl_link[1];
      s->avl_link[1] = p->avl_link[1];
      s->avl_bal = p->avl_bal;
      *q = s;
    }
  }

  avl_free(p);

  assert(k > 0);
  while (--k) {
    struct avl_node *s = pa[k];

    if (a[k] == 0) {
      if (s->avl_bal == -1)
        s->avl_bal = 0;
      else if (s->avl_bal == 0) {
        s->avl_bal = +1;
        break;
      } else /* |s->avl_bal == +1| */
      {
        struct avl_node *r = s->avl_link[1];
        assert(r != NULL);
        if (r->avl_bal == -1) {
          p = r->avl_link[0];
          r->avl_link[0] = p->avl_link[1];
          p->avl_link[1] = r;
          s->avl_link[1] = p->avl_link[0];
          p->avl_link[0] = s;
          if (p->avl_bal == +1)
            s->avl_bal = -1, r->avl_bal = 0;
          else if (p->avl_bal == 0)
            s->avl_bal = r->avl_bal = 0;
          else /* |p->avl_bal == -1| */
            s->avl_bal = 0, r->avl_bal = +1;
          p->avl_bal = 0;
          pa[k - 1]->avl_link[a[k - 1]] = p;
        } else {
          s->avl_link[1] = r->avl_link[0];
          r->avl_link[0] = s;
          pa[k - 1]->avl_link[a[k - 1]] = r;
          if (r->avl_bal == 0) {
            r->avl_bal = -1;
            break;
          } else
            s->avl_bal = r->avl_bal = 0;
        }
      }
    } else {
      if (s->avl_bal == +1)
        s->avl_bal = 0;
      else if (s->avl_bal == 0) {
        s->avl_bal = -1;
        break;
      } else {
        struct avl_node *r = s->avl_link[0];
        assert(r != NULL);
        if (r->avl_bal == +1) {
          p = r->avl_link[1];
          r->avl_link[1] = p->avl_link[0];
          p->avl_link[0] = r;
          s->avl_link[0] = p->avl_link[1];
          p->avl_link[1] = s;
          if (p->avl_bal == -1)
            s->avl_bal = +1, r->avl_bal = 0;
          else if (p->avl_bal == 0)
            s->avl_bal = r->avl_bal = 0;
          else /* |p->avl_bal == +1| */
            s->avl_bal = 0, r->avl_bal = -1;
          p->avl_bal = 0;
          pa[k - 1]->avl_link[a[k - 1]] = p;
        } else {
          s->avl_link[0] = r->avl_link[1];
          r->avl_link[1] = s;
          pa[k - 1]->avl_link[a[k - 1]] = r;
          if (r->avl_bal == 0) {
            r->avl_bal = +1;
            break;
          } else
            s->avl_bal = r->avl_bal = 0;
        }
      }
    }
  }

  return ( void * )item;
}

/* Initializes |trav| for |tree|.
   Returns data item in |tree| with the smallest value,
   or |NULL| if |tree| is empty.
   In the former case, |avl_next()| may be called with |trav|
   to retrieve additional data items. */
void *avl_first(struct avl_tree *tree, struct avl_traverser *trav)
{
  assert(tree != NULL && trav != NULL);
  trav->avl_node = tree->avl_root;
  trav->avl_height = 0;
  return avl_next(trav);
}

/* Returns the next data item in in-order
   within the tree being traversed with |trav|,
   or if there are no more data items returns |NULL|.
   In the former case |avl_next()| may be called again
   to retrieve the next item. */
void *avl_next(struct avl_traverser *trav)
{
  struct avl_node *node;

  assert(trav != NULL);
  node = trav->avl_node;
  for (;;) {
    while (node != NULL) {
      trav->avl_stack[trav->avl_height++] = node;
      node = node->avl_link[0];
    }

    if (trav->avl_height == 0)
      return NULL;

    node = trav->avl_stack[--trav->avl_height];
    trav->avl_node = node->avl_link[1];
    return node->avl_data;
  }
}

/* Copies |org| to a newly created tree, which is returned.
   If |copy != NULL|, each data item in |org| is first passed to |copy|,
   and the return values are inserted into the tree;
   |NULL| return values are taken as indications of failure.
   On failure, destroys the new tree,
   applying |destroy|, if non-null, to each item in the new tree so far,
   and returns |NULL|. */
struct avl_tree *avl_copy(const struct avl_tree *org, avl_copy_func *copy,
                          avl_node_func *destroy)
{
  struct avl_node *stack[2 * (AVL_MAX_HEIGHT + 1)];
  int height = 0;

  struct avl_tree *new;
  const struct avl_node *x;
  struct avl_node *y;

  new = avl_create(org->avl_compare, org->avl_param);
  if (new == NULL)
    return NULL;
  new->avl_count = org->avl_count;
  if (new->avl_count == 0)
    return new;

  x = ( const struct avl_node * )&org->avl_root;
  y = ( struct avl_node * )&new->avl_root;
  for (;;) {
    while (x->avl_link[0] != NULL) {
      y->avl_link[0] = avl_allocate(sizeof *y->avl_link[0]);
      if (y->avl_link[0] == NULL) {
        if (y != ( struct avl_node * )&new->avl_root) {
          y->avl_data = NULL;
          y->avl_link[1] = NULL;
        }
        goto error_handler;
      }

      stack[height++] = ( struct avl_node * )x;
      stack[height++] = y;
      x = x->avl_link[0];
      y = y->avl_link[0];
    }
    y->avl_link[0] = NULL;

    for (;;) {
      if (copy == NULL)
        y->avl_data = x->avl_data;
      else {
        y->avl_data = copy(x->avl_data, org->avl_param);
        if (y->avl_data == NULL) {
          y->avl_link[1] = NULL;
          goto error_handler;
        }
      }
      y->avl_bal = x->avl_bal;

      if (x->avl_link[1] != NULL) {
        y->avl_link[1] = avl_allocate(sizeof *y->avl_link[1]);
        if (y->avl_link[1] == NULL)
          goto error_handler;

        x = x->avl_link[1];
        y = y->avl_link[1];
        break;
      } else
        y->avl_link[1] = NULL;

      if (height <= 2)
        return new;

      y = stack[--height];
      x = stack[--height];
    }
  }

error_handler:
  for (; height > 2; height -= 2)
    stack[height - 1]->avl_link[1] = NULL;
  avl_destroy(new, destroy);
  return NULL;
}

/* Frees storage allocated for |tree|.
   If |destroy != NULL|, applies it to each data item in indeterminate
   order. */
void avl_destroy(struct avl_tree *tree, avl_node_func *destroy)
{
  struct avl_node *stack[AVL_MAX_HEIGHT];
  unsigned char state[AVL_MAX_HEIGHT];
  int height = 0;

  struct avl_node *p;

  assert(tree != NULL);
  p = tree->avl_root;
  for (;;) {
    while (p != NULL) {
      stack[height] = p;
      state[height] = 0;
      height++;

      p = p->avl_link[0];
    }

    for (;;) {
      if (height == 0) {
        avl_free(tree);
        return;
      }

      height--;
      p = stack[height];
      if (state[height] == 0) {
        state[height++] = 1;
        p = p->avl_link[1];
        break;
      } else {
        if (destroy != NULL && p->avl_data != NULL)
          destroy(p->avl_data, tree->avl_param);
        avl_free(p);
      }
    }
  }
}
