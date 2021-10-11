// void_ptr.c
/*
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT "foobar"

extern char *strcpy(char *, const char *);

int main(int argc, char **argv)
{
	int iStr_size = strlen(TEXT) + 1;
	char *pStr = NULL;
	if ((pStr = calloc(iStr_size, sizeof(*pStr))) == NULL)
		return EXIT_FAILURE;
	strcpy(pStr, TEXT);

	// void pointer
	void *pVoid;
	printf("init:\n\tiStr_size = %d\n\tpStr = \"%s\"\n", iStr_size, pStr);
	printf("\n");

	// pVoid -> char*
	printf("init pVoid to char*\n");
	pVoid = (char *)pStr;
	printf("\tpVoid = %p\tpStr = %p\n", pVoid, pStr);
	printf("\tpVoid = %s\tpStr = %s\n", (char *)pVoid, pStr);
	printf("\n");

	// pVoid -> int
	printf("init pVoid to int:\n");
	pVoid = (int *)&iStr_size;
	printf("\tpVoid = %p\tiStr_size = %lx\n", pVoid,
	       (unsigned long)&iStr_size);
	printf("\tpVoid = %d\tiStr_size = %d\n", *((int *)pVoid), iStr_size);
	printf("\n");

	// change value for pVoid, pVoid is still int!
	printf("change the value for pVoid (int*)\n");
	*((int *)pVoid) = 77;
	printf("\tpVoid = %p\tiStr_size = %lx\n", pVoid,
	       (unsigned long)&iStr_size);
	printf("\tpVoid = %d\tiStr_size = %d\n", *((int *)pVoid), iStr_size);

	printf("\n");

	// free
	printf("free()\n");
	if (pStr != NULL)
		free(pStr);
	printf("READY.\n");

	return EXIT_SUCCESS;
}
