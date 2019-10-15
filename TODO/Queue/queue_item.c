// queue_item.c

#include <stdio.h>
#include <stdlib.h>

#include "queue_item.h"

item *getNewItem(char *content)
{
  item *pItem = malloc(sizeof(item));
  if (pItem == NULL) {
    fprintf(stderr, "queue_item::getNewItem() - Out of Memory!\n");
    exit(8);
  };

  strcpy(pItem->content, content);
  pItem->next = NULL;

  return pItem;
};


void addItem(char *content)
{
  item *pItem = getNewItem(content);
  if (first == NULL) {
    first = pItem;
  } else {
    int size = queueSize();
    item *pTemp = NULL;
    do {
      --size;
      pTemp = getItemAt(size);
    } while ((pTemp == NULL) && (0 < size));
    if (pTemp == NULL) {
      fprintf(stderr, "queue_item::addItem() - Queue damaged!\n");
      exit(8);
    };
    pTemp->next = pItem;
  }
};


void removeItemAt(int idx)
{
  if (first == NULL)
    return;

  item *pItemBefore = NULL;
  item *pItemRemove = NULL;
  item *pItemAfter = NULL;

  if (idx == 0) {
    // pItemRemove == first
    if (first->next != NULL) {
      // ..and there are more items
      pItemAfter = first->next;
      free(first);
      first = pItemAfter;
    } else {
      // there are not more items
      free(first);
      first = NULL;
    }
  } else {
    // pItemRemove != first
    pItemBefore = getItemAt(idx - 1);
    if (pItemBefore == NULL) {
      fprintf(stderr, "queue_item::removeItemAt() - Queue damaged!\n");
      exit(8);
    };

    // pItemRemove is out of bounds:
    if (pItemBefore->next == NULL)
      return;

    pItemRemove = pItemBefore->next;
    if (pItemRemove->next == NULL) {
      // pItemRemove is the last elment of the queue
      pItemBefore->next == NULL;
      free(pItemRemove);
      pItemRemove = NULL;
    } else {
      // pItemRemove is not the last element
      pItemAfter = pItemRemove->next;
      pItemBefore->next = pItemAfter;
      free(pItemRemove);
      pItemRemove = NULL;
    }
  }
};


void insertItemAt(int idx, char content[])
{
  // queue is empty or idx is out of bounds
  if ((first == NULL) || (queueSize() <= idx) || (idx < 0)) {
    addItem(content);
    return;
  }

  item *pItem = getNewItem(content);

  // if idx == 0
  if (idx == 0) {
    if (queueSize() == 1) {
      addItem(content);
    } else {
      pItem->next = first->next;
      first = pItem;
    }
  } else {
    item *pBefore = getItemAt(idx - 1);
    if (pBefore == NULL) {
      fprintf(stderr, "queue_item::insertItemAt() - pBefore damaged!\n");
      return;
    };

    pItem->next = pBefore->next;
    pBefore->next = pItem;
  }
};


int queueSize()
{
  if (first == NULL)
    return 0;

  item *pItem = first;
  int cnt = 1;
  for (cnt = 1; pItem != NULL; pItem = pItem->next, ++cnt) {
    if (pItem->next == NULL)
      break;
  }
  return cnt;
};


item *getItemAt(int idx)
{
  if (first == NULL)
    return NULL;

  item *pItem = first;
  int cnt = 0;
  for (cnt = 0; pItem != NULL; pItem = pItem->next, ++cnt)
    if (idx == cnt)
      return pItem;

  return pItem;
}


char *getContent(int idx)
{
  if (first == NULL)
    return NULL;

  item *pItem = getItemAt(idx);
  if (pItem == NULL)
    return NULL;
  else
    return pItem->content;
};


int find(char content[], int length)
{
  item *pItem = first;
  int cnt = 0;

  while (pItem != NULL) {
    if (!strcmp(pItem->content, content)) {
      int i = 0;
      for (i = 0; i < length; ++i)
        if (pItem->content[i] != content[i])
          break;

      if (i == length)
        return cnt;
    }
    pItem = pItem->next;
    ++cnt;
  }
  return -1;
};
