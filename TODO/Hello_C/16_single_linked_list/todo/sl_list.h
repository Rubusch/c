// sl_list.h
/*
  headerfile for sl_list.c

  A header file serves as simple interface. The headerfile 
  contains declarations and prototypes of the functions 
  implemented in the program.

  A header file thus contains all declarations and once included 
  in a source file the used functions are already "known".

  A header file on the other side is a simple interface, the 
  stuff in a header file needs to be implemented, and other modules
  can rely on the functions in the header file to guarantee the 
  functionality.

  A single linked list works like that:
  
  element
    \next->element
             \next->element
	              \next->NULL

  The list is "single linked" because it only has one link to 
  the next element, this is easier to implement, but it has 
  certain limitations - especially iterating is only possible 
  in one direction.

  The element consists in a "struct" with one pointer to the next
  and one pointer to data (char). 
  A definition:

  struct element{
    char* data;
  };
  
  ...allways would need a definition of an instance like that:
  struct element my_element;
  
  ...therefore we "typedef" the whole definition of "struct element"
  to "element_t", see below. Therefore insight the struct we use the
  "struct element" expression because we still can't access the 
  "element_t" typedeffed expression.
//*/

#ifndef SINGLE_LINKED_LIST
#define SINGLE_LINKED_LIST

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATASIZ 64

typedef struct element{
  struct element* next;
  char* data;
} element_t;

// external usage interface level
int removeall();
int appendelement(const char*, const unsigned int);
int removelastelement();
int insertelement(const char*, const unsigned int, const char*, const unsigned int);
int removeelement(const char*, const unsigned int);
int printlist();

#endif
