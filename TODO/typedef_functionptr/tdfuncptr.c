// tdfuncptr.c
/*
//*/


#include <stdlib.h>

void some_function()
{
  ;
}


int main()
{
  typedef void (*funptr)();

  funptr p_some_function;

  p_some_function = &some_function;

  return 0;
}
