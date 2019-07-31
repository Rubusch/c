// helloworld.c
/*
  demonstrates the basic use of a pointer
  static array:
  char arr[7];
      the size is fixed, the memory will be on the stack, the
      memory is called "static memory" which has nothing to do with
      the keyword "static"!!!!

  char* arr;
      pointer, can be allocated dynamically to a number of elements
      it can be used like an array, though the size of the array is
      flexible: it's size can be shrunk and extended by more elements.
      the dynamic memory will be allocated on the heap! the pointer
      doesn't need obligatory be initialized to dynamic memory, it can
      also point to static memory, for example to a "normal" variable.

  dynmamic memory is allocated memory and needs to be freed afterwards!

  it can be allocated by (see manpages!):
      malloc(size)
      calloc(number_of_elements, size_of_elements)
      realloc(pointer, size)

  DYNAMIC MEMORY NEEDS TO BE FREED, IF NOT THE PROGRAM HAS A MEMORY LEAK!
  freeing a pointer that already has been freed or points to NULL, leads
  to an error:
  -> "double free", therefore try it always before, like this:

      if(NULL != pointer){ free(pointer); }
      pointer = NULL;

  this is not 100% but it's safer than nothing!

  Reset the pointer to NULL, because it will continue pointing on the
  address after a free. Just the memory isn't allocated anymore, the
  address still exists. This means, writing to the address is still
  possible, but we don't know who is using the address ("writing to
  somewhere").

  CAUTION: a pointer to a variable having static memory can't be free'd!!!!
  e.g:
  int foobar;
  int* ptr = &foobar;

  this pointer just points to the address of the variable "foobar", no allocation!
  a "free(ptr)" has to lead to an error!
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
  function to init the pointer

  when passing a pointer to an "array" allways pass the size of the array!
//*/
int func(char* ptr, int size)
{
  // initializing a string with "strncpy()"
  strncpy((char *)ptr, "blabla", size);

  // everything ok, return 0
  return 0;
}


int main()
{
  // allways init variables!
  char* ptr = NULL;
  int ptr_siz = 7;

  // allocate some memory
  if(NULL == (ptr = calloc( sizeof(*ptr), ptr_siz))){
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }

  // init the pointer in another function
  func(ptr, ptr_siz);

  // the result
  printf("pointer was \"%s\"\n", ptr);

  // free the allocated memory
  free(ptr);

  /*
    the stdlib defines the exit() function, which calls the _exit()
    function to help the operating system to do the clean up (ideally!).

    the two macros: EXIT_SUCCESS and EXIT_FAILURE are defined for that
    purpose
  //*/
  exit(EXIT_SUCCESS);
}
