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
	printf("expected: [empty]\n\n");

	puts("init 1");
	elements__append("AAA");
	elements__print();
	printf("expected: [AAA]\n\n");

	puts("append another element");
	elements__append("BBB");
	elements__print();
	printf("expected: [AAA, BBB]\n\n");

	puts("append another element");
	elements__append("CCC");
	elements__print();
	printf("expected: [AAA, BBB, CCC]\n\n");

	puts("remove element");
	elements__remove("BBB");
	elements__print();
	printf("expected: [AAA, CCC]\n\n");

	puts("remove non-existend element");
	elements__remove("BBB");
	elements__print();
	printf("expected: [AAA, CCC]\n\n");

	puts("remove all elements");
	elements__removeall();
	elements__print();
	printf("expected: [empty]\n\n");

	puts("READY.");
	exit(EXIT_SUCCESS);
}
