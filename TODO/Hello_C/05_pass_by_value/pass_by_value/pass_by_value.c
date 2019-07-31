// pass_by_value.c
/*
  demonstrates pass-by-value

  pass-by-value means passing only the content of a variable to a 
  subfunction, this is similar as passing only a copy of the variable 
  to the subfunction.

  when the subfunction changes the value, the calling function still
  has the original value

output:
parent:	777

	 *** subfunction called *** 

	passed value (before):	777

	 *** change value *** 

	passed value (after):	-223

	 *** subfunction done *** 

parent:	777
parent: READY.
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// forward declaration
void subfunction(int);


int main()
{
  // initializing the value
  int value = 777;
  printf("parent:\t%d\n", value);

  // calling the function: pass-by-value
  subfunction(value);

  printf("parent:\t%d\n", value);

  puts("parent: READY.");
  exit(EXIT_SUCCESS);
}


/*
  called subfunction will get a copy of the value
//*/
void subfunction(int arg)
{
  puts("\n\t *** subfunction called *** \n");
  printf("\tpassed value (before):\t%d\n", arg);

  puts("\n\t *** change value *** \n");
  // change value
  arg = arg - 500 * 2;

  printf("\tpassed value (after):\t%d\n", arg);
  puts("\n\t *** subfunction done *** \n");
}
