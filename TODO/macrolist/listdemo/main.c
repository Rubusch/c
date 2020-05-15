// main.c
/*
  main file to start the proggy
//*/

#include <stdio.h>
#include <stdlib.h>

#include "macrolist.h"


int main()
{
	puts("uninitialized");
	elements__print();
	puts("");

	puts("init");
	elements__append("AAA");
//	elements__append("BBB");
//	elements__append("CCC");
	elements__print();
	puts("");

	puts("remove element");
	elements__remove("BBB");
	elements__print();
	puts("");

	puts("should fail");
	elements__remove("BBB");
	elements__print();
	puts("");

	puts("cleanup");
	elements__removeall();
	elements__print();
	puts("");

	exit(EXIT_FAILURE);
}
