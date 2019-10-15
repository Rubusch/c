// insertionsort.c
/*
  Insertion sort is in-place (doesn't need further ressources proportional to
the amount of elements to sort).

  Insertion sort can be stable (an alphabetic order of elements, equal by
another key, e.g. birth dates sorted another time by that second key, remains
the same alphabetic order).
//*/


// for the element type
#include "main.h"

// header
#include "insertionsort.h"


/*
  insertion sort function
//*/
void insert(void *tmp)
{
  void *prev = getprevelement(tmp);
  while (NULL != prev) {
    if (compare(prev, tmp)) { // sorting order
      swap(prev, tmp);
    } else {
      break;
    }
    prev = getprevelement(tmp);
  }
}


/*
  sorting
//*/
void sort(void **first)
{
  void *iterator = *first;
  void *tmp = NULL;

  // check if first is only one element
  if (isfirstelement(*first) && islastelement(*first)) {
    return;
  }

  while (NULL != iterator) {
    tmp = iterator;
    insert(tmp);
    iterator = getnextelement(iterator);
  }

  // resetting first


  while (!isfirstelement(tmp)) {
    tmp = getprevelement(tmp);
  }
  *first = tmp;
}


/*
  comparator
//*/
int compare(void *arg_a, void *arg_b)
{
  element_t *elem_a = ( element_t * )arg_a;
  element_t *elem_b = ( element_t * )arg_b;

  unsigned int a = elem_a->data;
  unsigned int b = elem_b->data;

  if (a > b)
    return 1;
  else
    return 0;
}


/*
  swap elements
//*/
int swap(void *elem_a, void *elem_b)
{
  element_t *a = ( element_t * )elem_a;
  element_t *b = ( element_t * )elem_b;

  /*
    swapping within a double linked list, needs to redo 6 links
  //*/
  a->next = b->next; // can be NULL!
  b->prev = a->prev; // can be NULL!

  if (b->next)
    b->next->prev = a;
  b->next = a;

  if (a->prev)
    a->prev->next = b;
  a->prev = b;

  return 0;
}


/*
  is first element?
//*/
int isfirstelement(void *arg)
{
  element_t *tmp = ( element_t * )arg;

  if (NULL == tmp->prev)
    return 1;
  else
    return 0;
}


/*
  is last element
//*/
int islastelement(void *arg)
{
  element_t *tmp = ( element_t * )arg;

  if (NULL == tmp->next)
    return 1;
  else
    return 0;
}


/*
  next one
//*/
void *getnextelement(void *arg)
{
  if (arg == NULL)
    return NULL;
  element_t *tmp = ( element_t * )arg;

  return tmp->next;
}


/*
  previous one
//*/
void *getprevelement(void *arg)
{
  if (arg == NULL)
    return NULL;

  element_t *tmp = ( element_t * )arg;

  return tmp->prev;
}
