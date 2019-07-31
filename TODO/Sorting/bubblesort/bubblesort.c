// bubblesort.c
/*
  Bubblesort is in-place (doesn't need further ressources proportional to the amount of 
  elements to sort).

  Bubblesort is stable (an alphabetic order of elements, equal by another key, e.g. 
  birth dates sorted another time by that second key, remains in the same alphabetic 
  order).
//*/


// for the element type
#include "main.h"

// header
#include "bubblesort.h"



/*
  sorting function - this function should work completely abstracted from the data type to sort
//*/
void* sort(void** first)
{
  void* tmp = *first;

  char done = 0;

  while(!done){
    done = 1;
    tmp = *first;

    // while there is a next compare "tmp" and "tmp->next", in case swap
    while(!islastelement(tmp)){
      if(compare(tmp, getnextelement(tmp))){
	swap((void*) &tmp, getnextelement(tmp));
	done = 0;
      }

      tmp = iterate(tmp, first);
    }
  }

  return NULL;
}


/*
  check if the element is the first element
//*/
int isfirstelement(void* arg)
{
  element_t* tmp = (element_t*) arg;

  if(NULL == tmp->prev) return 1;
  else return 0;
}


/*
  check if the element is the last element
//*/
int islastelement(void* arg)
{
  element_t* tmp = (element_t*) arg;

  if(NULL == tmp->next) return 1;
  else return 0;
}


/*
  return the next element
//*/
void* getnextelement(void* arg)
{
  element_t* tmp = (element_t*) arg;

  return tmp->next;
}


/*
  iterates to the following element
//*/
void* iterate(void* arg, void** first)
{
  element_t* tmp = (element_t*) arg;
  
  // check for first element and set first
  if(isfirstelement(tmp)){
    *first = tmp;
  }

  // return the following element
  if(islastelement(tmp)){
    return *first;
  }else{
    return getnextelement(tmp);
  }
}


/*
  comparator, takes the content of the two elements to compare
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
  swaps the elements, the first counter needs to be the iterator index
//*/
int swap(void** elem_a, void* elem_b)
{
  element_t* a = *((element_t**) elem_a);
  element_t* b = (element_t*) elem_b;

  // counter
  element_t** tmp = (element_t**) elem_a;

  /*
    swapping within a double linked list, needs to redo 6 links
  //*/
  a->next = b->next; // can be NULL!  
  b->prev = a->prev; // can be NULL!


  if(b->next) b->next->prev = a;
  b->next = a;

  if(a->prev) a->prev->next = b;
  a->prev = b;
  
  *tmp = b;

  return 0;
}
