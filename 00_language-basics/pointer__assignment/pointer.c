// pointer_test.c
/*
  demonstrates strlen() and sizeof(a)/sizeof(char)
  demonstrates the use of pointers and double pointers on chars
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int val = 123;
	puts("int val = 123;");
	printf("val:\t%i, address:\t0x%lX\n", val, (unsigned long)&val);
	puts("");

	int *pIntR = &val;
	puts("int *pIntR = &val; // rvalue init");
	printf("*pIntR:\t%i, pIntR:\t0x%lX, &pIntR:\t0x%lX\n", *pIntR,
	       (unsigned long)pIntR, (unsigned long)&pIntR);
	puts("");

	int *pIntL = NULL;
	pIntL = &val;
	puts("pIntL = &val; // lvalue init");
	printf("*pIntL:\t%i, pIntL:\t0x%lX, &pIntL:\t0x%lX\n", *pIntL,
	       (unsigned long)pIntL, (unsigned long)&pIntL);
	puts("");

	char ch = 'A';
	printf("ch (char):\t%c, ch (num):\t%i, address:\t0x%lX\n", ch, (char)ch,
	       (unsigned long)&ch);
	puts("");

	char *pChrR = &ch;
	puts("char *pChrR = &ch; // rvalue init");
	printf("*pChrR (c):\t%c, *pChrR (n):\t%i, pChrR:\t0x%lX, &pChrR:\t0x%lX\n",
	       *pChrR, (char)*pChrR, (unsigned long)pChrR,
	       (unsigned long)&pChrR);
	puts("");

	char *pChrL = NULL;
	pChrL = &ch;
	puts("pChrL = &ch; // lvalue init");
	printf("*pChrL (c):\t%c, *pChrL (n):\t%i, pChrL:\t0x%lX, &pChrL:\t0x%lX\n",
	       *pChrL, (char)*pChrL, (unsigned long)pChrL,
	       (unsigned long)&pChrL);
	puts("");

	return EXIT_SUCCESS;
}
