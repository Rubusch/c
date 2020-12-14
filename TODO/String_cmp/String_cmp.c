// String_cmp
/*
        avoid strcpy() -> use strncpy()
        avoid strcmp() -> use strncmp()
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT01 "foobar"
#define TEXT02 "barfoo"
//#define TEXT02 "FOOBAR"

extern char *strcpy(char *, const char *);
extern int strcmp(const char *, const char *);


int string_cmp(char *str1, char *str2, int *pResult)
{
  // modifications of strcmp can be tested here!
  *pResult = strcmp(str1, str2);
  return 0;
}


int main(int argc, char **argv)
{
	char *pText01 = NULL;
	char *pText02 = NULL;

	printf("init\n");
	if ((pText01 = malloc(sizeof(pText01))) == NULL)
		goto fail;
	strcpy(pText01, TEXT01); // easier with dup(), dup2() ?!
	if ((pText02 = malloc(sizeof(pText02))) == NULL)
		goto fail;
	strcpy(pText02, TEXT02);


	// strcmp output
	printf("do string_cmp()\n");
	int iResult = 0;
	if (-1 == string_cmp(pText01, pText02, &iResult))
		goto fail;
	printf("strcmp: \"%s\" and \"%s\", \nresult: \'%d\' - ", pText01, pText02,
	       iResult);

	// print result
	if (iResult < 0) {
		printf("\"%s\" greater than \"%s\"\n", pText01, pText02);
	} else if (iResult > 0) {
		printf("\"%s\" smaller than \"%s\"\n", pText01, pText02);
	} else {
		printf("Equal!\n");
	}


	if (pText01 != NULL)
		free(pText01);
	if (pText01 != NULL)
		free(pText02);

	printf("READY.\n");

	return EXIT_SUCCESS;

fail:
	if (pText01) free(pText01);
	pText01 = NULL;
	if (pText02) free(pText02);
	pText02 = NULL;

	return EXIT_FAILURE;
}
