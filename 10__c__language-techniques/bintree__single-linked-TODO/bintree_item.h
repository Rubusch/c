// bintree_item.h
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 */
#ifndef BINTREE_ITEM
#define BINTREE_ITEM

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CONTENT_LENGTH 8

struct bintree_item {
  struct bintree_item *lhs;
  struct bintree_item *rhs;
  char content[CONTENT_LENGTH];
};
typedef struct bintree_item item;

void generateTree(char[]);
void addItemSorted(char[]);
item *findItem(char[]);

#endif
