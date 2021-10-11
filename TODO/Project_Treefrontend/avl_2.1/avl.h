/*=+cch======================================================================*/
/*  NAME     : avl.h                                                         */
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

#ifndef AVL_H
#define AVL_H 1

#include <stddef.h>

/* Data item comparison function. */
typedef int avl_comparison_func(const void *avl_a, const void *avl_b,
				void *avl_param);
typedef void avl_node_func(void *bst_data, void *bst_param);
typedef void *avl_copy_func(void *avl_data, void *avl_param);

/* An AVL tree node. */
struct avl_node {
	struct avl_node *avl_link[2]; /* Subtrees. */
	void *avl_data; /* Pointer to data. */
	signed char avl_bal; /* Balance factor. */
	unsigned char avl_cache; /* Caches comparisons on insertion. */
	char avl_pad[2]; /* Unused.  Reserved for threaded trees. */
};

/* A binary search tree. */
struct avl_tree {
	struct avl_node *avl_root; /* Tree's root. */
	avl_comparison_func *avl_compare; /* Comparison function. */
	void *avl_param; /* Extra argument to |avl_compare|. */
	size_t avl_count; /* Number of items in tree. */
};

#ifndef AVL_MAX_HEIGHT
#define AVL_MAX_HEIGHT 32
#endif

struct avl_traverser {
	struct avl_node *avl_node;
	struct avl_node *avl_stack[AVL_MAX_HEIGHT];
	size_t avl_height;
};

void *avl_allocate(size_t);
void avl_free(void *);
struct avl_tree *avl_create(avl_comparison_func *, void *);
#define AVL_MATCH 0
#define AVL_GE 1
#define AVL_LE 2
void *avl_find(const struct avl_tree *, const void *, int flag);
void *avl_find_and_traverser_init(const struct avl_tree *, const void *,
				  int flag, struct avl_traverser *);
void *avl_insert(struct avl_tree *, void *);
void *avl_replace(struct avl_tree *, void *);
void *avl_delete(struct avl_tree *, const void *);
void *avl_first(struct avl_tree *, struct avl_traverser *);
void *avl_next(struct avl_traverser *);
struct avl_tree *avl_copy(const struct avl_tree *org, avl_copy_func *copy,
			  avl_node_func *destroy);
void avl_destroy(struct avl_tree *, avl_node_func *);

/* Returns the number of items in |TREE|. */
#define avl_count(TREE) ((size_t)(TREE)->avl_count)

/* Easy assertions for insertion and deletion. */
#ifdef NDEBUG
void avl_assert_insert(struct avl_tree *, void *);
void *avl_assert_delete(struct avl_tree *, void *);
#else
#define avl_assert_insert(TREE, ITEM) avl_insert(TREE, ITEM)
#define avl_assert_delete(TREE, ITEM) avl_delete(TREE, ITEM)
#endif

#endif /* avl.h */
