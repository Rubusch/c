// main.c
/*
  main file to start the proggy
//*/

#include <stdio.h>
#include <stdlib.h>

#include "macrolist.h"

#ifndef GPIOS_LIST
#define GPIOS_LIST
CREATE_LIST(gpios)
#endif

int main()
{
	puts("uninitialized");
	gpios__print();
	printf("expected: [empty]\n\n");

	puts("init 1");
	gpios__append("D01");
	gpios__print();
	printf("expected: [D01]\n\n");

	puts("append another element");
	gpios__append("D02");
	gpios__print();
	printf("expected: [D01, D02]\n\n");

	puts("append another element");
	gpios__append("D03");
	gpios__print();
	printf("expected: [D01, D02, D03]\n\n");

	puts("remove element");
	gpios__remove("D02");
	gpios__print();
	printf("expected: [D01, D03]\n\n");

	puts("remove non-existend element");
	gpios__remove("D02");
	gpios__print();
	printf("expected: [D01, D03]\n\n");

	puts("remove all gpios");
	gpios__removeall();
	gpios__print();
	printf("expected: [empty]\n\n");

	exit(EXIT_FAILURE);
}
