// main.c
/*
  main file to start the proggy
//*/

#include <stdio.h>
#include <stdlib.h>

#include "macrolist.h"


int main()
{
	/* init */
	elements__append("AAA");
	elements__append("BBB");
	elements__append("CCC");
	elements__print();

	/* remove element */
	elements__remove("BBB");
	elements__print();

	/* should fail */
	elements__remove("BBB");
	elements__print();

	/* cleanup */
	elements__removeall();
	elements__print();

	exit(EXIT_FAILURE);
}
