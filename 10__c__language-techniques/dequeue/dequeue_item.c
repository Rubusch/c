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
static int size;

/*    
static item *getNextItem(item *);
static item *getPrevItem(item *);
// */


/* private */

item_p dequeue__new(content_t* content)
{
  item_p item = malloc(sizeof(item));
  if (item == NULL) {
    fprintf(stderr, "dequeue_item::dequeue__new() - bad allocation!\n");
    exit(8);
  }
  item->content = content;

  return item;
};


/* public */

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

item_p dequeue__next(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->next;
};

item_p dequeue__prev(item_p elem)
{
	if (elem == NULL)
		return NULL;
	return elem->prev;
}

void dequeue__append(content_p content)
{
	if (first == NULL) {
		last = dequeue__new(content);
		first = last;
	} else {
		item_p item = dequeue__new(content);
		last->next = item;
		item->prev = last;
		last = last->next;
	}

	size++;
}

void dequeue__remove(item_p elem)
{
	/* NB: make sure content is free*d before */
	item_p elem_before = NULL;
	if (elem != first)
		elem_before = dequeue__prev(elem);

	item_p elem_after = NULL;
	if (elem != last)
		elem_after = dequeue__next(elem);

	if (elem_before != NULL || elem_after != NULL) {
		elem_before->next = elem_after;
		elem_after->prev = elem_before;
	} else if (elem_before == NULL && elem_after != NULL) {
		first = dequeue__next(first);
		first->prev = NULL;
	} else if (elem_after == NULL && elem_before != NULL) {
		last = dequeue__prev(last);
		last->next = NULL;
	} else {
		/* remove: first == last */
		first = NULL;
		last = NULL;
	}
	free(elem);

	size--;
}




/*
void insertItemAt(int idx, char content[])
{
  item *pItem = dequeue__new(content);
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

  item *elem_before = getItemAt(idx - 1);
  item *elem_after = getItemAt(idx);

  elem_before->next = pItem;
  pItem->prev = elem_before;

  pItem->next = elem_after;
  elem_after->prev = pItem;
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
      pItem = dequeue__next(pItem);

  } else {
    pItem = last;
    int cnt = size - 1;
    for (cnt = size - 1; (cnt != idx) && (cnt >= 0); --cnt) {
      pItem = dequeue__prev(pItem);
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


// */
