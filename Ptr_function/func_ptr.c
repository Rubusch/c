// func_ptr.c
/*
  function pointer demo
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUT 0
#define OUTPUT 1

// macro ;)
#define IO(x) (*ptr_arr[x])


int main(int argc, char** argv)
{
  printf(" *** function pointer on printf ***\n");

  // function pointer - init
  int (*ptr) (const char*, ...);

  // init the function pointer with the func "printf(const char*, ...)"
  ptr = printf;

  // use the function pointer, as the function itself
  (*ptr)("Print out %i and use the function pointer\n", 777);

  printf("\n\n *** function pointer array on printf and scanf ***\n\n");

  // func ptr array - init
  int (*ptr_arr[]) (const char*, ...) = {scanf, printf};
  int iVal;
  /*
    for accepting a function to an array of func pointers the RETURN value 
    compares to the type of a variable in a common array.
  //*/

  // usage
  (*ptr_arr[OUTPUT])("Enter a number:\n");
  (*ptr_arr[INPUT])("%d", &iVal);
  (*ptr_arr[OUTPUT])("The number was: %i\n", iVal);

  // use the macro
  IO(1)("READY.\n");
  return EXIT_SUCCESS;
}
