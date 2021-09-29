// dequeue_item.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */
// TODO make first public  
// TODO make last public  
// TODO make content a mere pointer  

#include "dequeue_item.h"


static item_p first;
static item_p last;
static int size = 0;

/*
static item *getNextItem(item *);
static item *getPrevItem(item *);
// */

int dequeue__size()
{
	return size;
}

item_p dequeue__first()
{
	return first;
}

item_p dequeue__last()
{
	return last;
}


/*
item *getNewItem(char content[])
{
  item *pItem = malloc(sizeof(item));
  if (pItem == NULL) {
    fprintf(stderr, "dequeue_item::getNewItem() - bad allocation!\n");
    exit(8);
  }
  strcpy(pItem->content, content);

  return pItem;
};


void dequeue__append(char content[])
{
  if (first == NULL) {
    last = getNewItem(content);
    first = last;
  } else {
    item *pItem = getNewItem(content);
    last->next = pItem;
    pItem->prev = last;
    last = last->next;
  }
};


void removeItemAt(int idx)
{
  if ((idx < 0) || (idx >= dequeue__size()))
    return;

  item *pItem = getItemAt(idx);

  item *pBefore = NULL;
  if (pItem != first)
    pBefore = getPrevItem(pItem);

  item *pAfter = NULL;
  if (pItem != last)
    pAfter = getNextItem(pItem);


  if ((pBefore != NULL) || (pAfter != NULL)) {
    pBefore->next = pAfter;
    pAfter->prev = pBefore;
  } else if (pBefore == NULL) {
    first = getNextItem(first);
    first->prev = NULL;
  } else if (pAfter == NULL) {
    last = getPrevItem(last);
    last->next = NULL;
  }

  free(pItem);
  pItem = NULL;
};


void insertItemAt(int idx, char content[])
{
  item *pItem = getNewItem(content);
  if (idx <= 0) {
    pItem->next = first;
    first->prev = pItem;
    first = pItem;
    return;
  }

  if (idx >= dequeue__size()) {
    dequeue__append(content);
    return;
  }

  item *pBefore = getItemAt(idx - 1);
  item *pAfter = getItemAt(idx);

  pBefore->next = pItem;
  pItem->prev = pBefore;

  pItem->next = pAfter;
  pAfter->prev = pItem;
};
// */

/*

                 

item *getItemAt(int idx)
{
  int size = dequeue__size();
  if ((idx >= size) || (idx < 0))
    return NULL;

  item *pItem = NULL;
  if (idx + 1 <= size / 2) {
    pItem = first;
    int cnt = 0;
    for (cnt = 0; (cnt != idx) && (cnt < size); ++cnt)
      pItem = getNextItem(pItem);

  } else {
    pItem = last;
    int cnt = size - 1;
    for (cnt = size - 1; (cnt != idx) && (cnt >= 0); --cnt) {
      pItem = getPrevItem(pItem);
    }
  }

  return pItem;
};


char *getContentAt(int idx)
{
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


static item *getNextItem(item *pItem)
{
  if (pItem == NULL)
    return NULL;
  return pItem->next;
};


static item *getPrevItem(item *pItem)
{
  if (pItem == NULL)
    return NULL;
  return pItem->prev;
};
// */
