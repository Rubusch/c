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
void btree_search()
{
	// TODO
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
	// TODO
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
void btree_split_child()
{
	// TODO
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
void btree_insert()
{
	// TODO
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
void btree_insert_nonfull()
{
	// TODO
}
