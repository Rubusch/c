// selectionsort.c
/*
  Selection sort is in-place (doesn't need further ressources proportional to the amount of 
  elements to sort).

  Selection sort is NOT stable (an alphabetic order of elements, equal by another key, e.g. 
  birth dates sorted another time by that second key, doesn't need to be the same alphabetic 
  order). It can be implemented stable, too.
//*/


// for the element type
#include "main.h"

// header
#include "selectionsort.h"


/*
  find the minimum elment, starting from "first" and using the compare function, and returns 
  it - otherwise returns NULL
//*/
void* find_minimum(void* first)
{
  if(NULL == first) return NULL;

  element_t* min = (element_t*) first;
  element_t* iterator = getnextelement(min);

  while(NULL != iterator){
    if(0 < compare(min, iterator)){
      min = iterator;
    }
    iterator = getnextelement(iterator);
  }
  return min;
}


/*
  exchanges an element that not obligatory needs to be a neighbor

  the elements will be switched in their order and the pointers will be reset so that 
  an impression will be invoked that only the contents of the elements have changed.
//*/
int exchange(void** arg_a, void** arg_b)
{
  if(NULL == arg_a) return -1;
  if(NULL == arg_b) return -1;

  element_t* elem_a = *((element_t**) arg_a);
  element_t* elem_b = *((element_t**) arg_b);

  if((elem_a->next == elem_b) || (elem_b->next == elem_a)){
    // in case of direct neighborhood
    swap(elem_a, elem_b);

  }else{
    // else, the light-weight pointer attempt to make it easier
    element_t* prev_a = elem_a->prev;
    element_t* next_a = elem_a->next;
    element_t* prev_b = elem_b->prev;
    element_t* next_b = elem_b->next;

    elem_a->prev = prev_b;
    elem_a->next = next_b;
    elem_b->prev = prev_a;
    elem_b->next = next_a;

    if(NULL != prev_a) prev_a->next = elem_b;
    if(NULL != next_a) next_a->prev = elem_b;
    if(NULL != prev_b) prev_b->next = elem_a;
    if(NULL != next_b) next_b->prev = elem_a;
  }

  // reset the pointers
  *arg_a = elem_b;
  *arg_b = elem_a;
  
  return 0;
}


/*
  sorting
//*/
void sort(void** first)
{
  if(NULL == first) return;

  void* iterator = *first;
  void* tmp = NULL;

  while(1){
    tmp = find_minimum(iterator);
    exchange((void**) &iterator, (void**) &tmp);
    if(isfirstelement(iterator)) *first = iterator;
    iterator = getnextelement(iterator);
    if(islastelement(iterator)) break;
  }
}


/*
  comparator
  returns 1 if a > b, else 0
//*/
int compare(void* arg_a, void* arg_b)
{
  element_t* elem_a = (element_t*) arg_a;
  element_t* elem_b = (element_t*) arg_b;

  unsigned int a = elem_a->data;
  unsigned int b = elem_b->data;

  if(a > b) return 1;
  else return 0;
}


/*
  swap elements
//*/
int swap(void* elem_a, void* elem_b)
{
  element_t* a = (element_t*) elem_a;
  element_t* b = (element_t*) elem_b;

  /*
    swapping within a double linked list, needs to redo 6 links
  //*/
  a->next = b->next; // can be NULL!  
  b->prev = a->prev; // can be NULL!

  if(b->next) b->next->prev = a;
  b->next = a;

  if(a->prev) a->prev->next = b;
  a->prev = b;

  return 0;
}


/*
  is first element?
//*/
int isfirstelement(void* arg)
{
  element_t* tmp = (element_t*) arg;

  if(NULL == tmp->prev) return 1;
  else return 0;
}


/*
  is last element
//*/
int islastelement(void* arg)
{
  element_t* tmp = (element_t*) arg;

  if(NULL == tmp->next) return 1;
  else return 0;
}


/*
  next one
//*/
void* getnextelement(void* arg)
{
  if(arg == NULL) return NULL;
  element_t* tmp = (element_t*) arg;

  return tmp->next;
}


/*
  previous one
//*/
void* getprevelement(void* arg)
{
  if(arg == NULL) return NULL;

  element_t* tmp = (element_t*) arg;
 
  return tmp->prev;
}
