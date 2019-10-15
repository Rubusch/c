// pass_by_reference.c
/*
  Pass by reference means passing the variable directly and not
  just a copy of it's content.
  When we want to pass a variable by reference, we need to pass
  the address in memory (its reference) of the variable to the
  subfunction.
  Changes done by the subfunction will remain in the parent's
  value!
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// forward declaration
void subfunction(int *);


int main()
{
  // initializing the value
  int value = 777;
  printf("parent:\t%d\n", value);

  // calling the function: pass-by-value
  subfunction(&value);

  printf("parent:\t%d\n", value);

  puts("parent: READY.");
  exit(EXIT_SUCCESS);
}


/*
  called subfunction will get a copy of the value
//*/
void subfunction(int *arg)
{
  puts("\n\t *** subfunction called *** \n");
  printf("\tpassed value (before):\t%d\n", *arg);

  puts("\n\t *** change value *** \n");
  // change value
  *arg = *arg - 500 * 2;

  printf("\tpassed value (after):\t%d\n", *arg);
  puts("\n\t *** subfunction done *** \n");
}
