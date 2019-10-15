// quicksort.c
/*
  Classical recursive quicksort implementation running on a queue. Invoking
threads on the single steps might give an enhancement performance tremendously.

  Using quicksort rather on an array like structure seems to be faster since the
elements can be indexed directly - an intermediate layer here for a linked list
/ queue might be the implementation of an iterator, to provide a similar
  mechanism easily.

  Quicksort can be implemented iterative or recursive, though it's a classic
example of recursiveness.

  This is a recursive implementation of a quicksort using the first element as
pivot element.
//*/


// for the element_t
#include "main.h"

// header
#include "quicksort.h"

/*
  interchanges the element
//*/
int interchange(void **elem1, void **elem2)
{
  if (NULL == elem1)
    return -1;
  if (NULL == elem2)
    return -1;
  if (elem1 == elem2)
    return -1;

  element_t *ptr_a = *(( element_t ** )elem1);
  element_t *ptr_b = *(( element_t ** )elem2);

  while (1) {
    if ((ptr_b == ptr_a->next) || (ptr_a == ptr_b->next)) {
      swap(ptr_a, ptr_b);
      break;
    }

    // init
    element_t *a_next = ptr_a->next;
    element_t *a_prev = ptr_a->prev;
    element_t *b_next = ptr_b->next;
    element_t *b_prev = ptr_b->prev;

    // setting up ptr_b
    ptr_b->next = a_next;
    ptr_b->prev = a_prev;
    ptr_a->next = b_next;
    ptr_a->prev = b_prev;

    // setting environment
    if (b_next)
      b_next->prev = ptr_a;
    if (b_prev)
      b_prev->next = ptr_a;
    if (a_next)
      a_next->prev = ptr_b;
    if (a_prev)
      a_prev->next = ptr_b;

    break;
  }

  // resetting of ptrs
  *elem1 = ptr_b;
  *elem2 = ptr_a;

  return 0;
}


/*
  part of the recursive approach of a quicksort implementation

  iterate thru bottom half and push higher elements to upper half (before pivot)
- multithreading possible
//*/
void quicksort(void *first, void *last)
{
  // checks
  if (NULL == first)
    return;
  if (NULL == last)
    return;
  if (isfirstelement(last))
    return;
  if (islastelement(first))
    return;
  if (first == last)
    return;

  // ok, init
  void *smaller = first;
  void *pivot = last;
  void *greater = getprevelement(pivot);
  if (smaller == greater)
    return;

  // freeze the pointers to the position
  int freeze_greater = 0;
  int freeze_smaller = 0;

  while (1) {
    if (!freeze_smaller) {
      // compare: pivot < smaller
      if (0 < compare(smaller, pivot)) {
        // freeze
        freeze_smaller = 1;

      } else {
        smaller = getnextelement(smaller);
      }
    }

    if (!freeze_greater) {
      // compare: pivot > greater
      if (0 < compare(pivot, greater)) {
        // freeze
        freeze_greater = 1;

      } else {
        greater = getprevelement(greater);
      }
    }

    // break out
    if (smaller == greater)
      break;
    if (smaller == getnextelement(greater))
      break;
    if (greater == getprevelement(smaller))
      break;

    // both are frozen - interchange
    if (freeze_smaller && freeze_greater) {
      if (smaller == first)
        first = greater; // set new first
      if (0 > interchange(&smaller, &greater)) {
        perror(
            "something in the queue was NULL?!"); // this is impossible if the
                                                  // implementation is ok!
        return;
      }

      smaller = getnextelement(smaller);
      greater = getprevelement(greater);
      freeze_smaller = 0;
      freeze_greater = 0;
    }
  }

  // swap pivot with smaller
  if (freeze_greater && (0 > compare(pivot, greater))) {
    if (0 > interchange(&pivot, &greater))
      return;
    last = pivot;    // set the new last
    pivot = greater; // set the new pivot
  } else {
    if (0 == compare(smaller, pivot)) {
      smaller = getnextelement(smaller);
    }
    if (smaller == first)
      first = pivot; // set the new pivot
    if (0 > interchange(&smaller, &pivot))
      return;
    last = pivot;    // set the new last
    pivot = smaller; // set the new pivot
  }

  // split into:  first - (pivot-1) and pivot - last
  // everything already ordered
  if (pivot == last)
    return;

  // call for lower half
  quicksort(first, getprevelement(pivot));

  // call for upper half
  quicksort(pivot, last);
}


/*
  sorting

  calls the recursive function
//*/
void sort(void **first)
{
  // checks
  if (NULL == first)
    return;
  if (isfirstelement(*first) && islastelement(*first))
    return;

  // inits
  void *first_elem = *first;
  void *last_elem = first_elem;
  while (!islastelement(last_elem)) {
    last_elem = getnextelement(last_elem);
  }

  // actual sort (recursive)
  quicksort(first_elem, last_elem);

  // reset *first to the new first;
  while (!isfirstelement(first_elem)) {
    first_elem = getprevelement(first_elem);
  }
  *first = first_elem;
}


/*
  comparator
  returns 1 if a > b, 0 if equal or smaller,
  in case of error - returns -1
//*/
int compare(void *arg_a, void *arg_b)
{
  if (NULL == arg_a)
    return -1;
  if (NULL == arg_b)
    return -1;

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
  swap neighbor elements - elem_a MUST be the lower (prev-er / lefter) one,
elem_b the higher (next-er / righter) one!!
//*/
int swap(void *elem_a, void *elem_b)
{
  element_t *a = ( element_t * )elem_a;
  element_t *b = ( element_t * )elem_b;

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
