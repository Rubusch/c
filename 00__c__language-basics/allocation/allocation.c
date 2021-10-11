// allocation.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char *strcat(char *, const char *); // better strncat -> size == safer!!
extern char *strncpy(char *, const char *, size_t);

/*
  improved reallocation,
  returns 0 in case of success and -1 in case of error
//*/
int get_more_space(char **str, unsigned int *str_size,
		   const unsigned int how_much_more)
{
	char *tmp = NULL;

	// using realloc makes using strcat() here unnecessary
	// realloc() frees *str ???
	if ((tmp = realloc(*str, (*str_size + how_much_more))) == NULL) {
		if (tmp != NULL)
			free(tmp);
		tmp = NULL;
		return -1;
	}
	// using realloc makes free(*str) here unnecessary
	*str = tmp;
	*str_size += how_much_more;

	return 0;
}

/*
  allocates a char*, inits it and sets its size
//*/
int init_text(char **pResult, char *pText, unsigned int *pSize)
{
	if (pText == NULL)
		return -1;
	if (pSize == NULL)
		return -1;

	*pSize = (strlen(pText) + 1);
	if ((*pResult = calloc(*pSize, sizeof(**pResult))) == NULL)
		return -1;
	strcpy(*pResult, pText);

	return 0;
}

int init_value(int **pResult, const int value)
{
	if ((*pResult = malloc(sizeof(**pResult))) == NULL)
		return -1;
	**pResult = value;

	return 0;
}

/*
  allocates memory and copies into a new char*, if it works out, it
  changes the old size value to the new value

  COPY_SIZE = number of the char's to copy, starting from index '0'
  pOld_size = size of the old string inclusive the '\0'
  NEW_SIZE = size of the new string inclusive '\0'
//*/
int trim_char(char **pStr, const unsigned int COPY_SIZE,
	      unsigned int *pOld_size, const unsigned int NEW_SIZE)
{
	if (*pStr == NULL)
		return -1;
	if (COPY_SIZE > *pOld_size)
		return -1;
	if (COPY_SIZE > NEW_SIZE)
		return -1;

	char *pNewStr = NULL;

	if ((pNewStr = calloc(NEW_SIZE, sizeof(*pNewStr))) == NULL)
		return -1;
	if ((pNewStr = strncpy(pNewStr, *pStr, COPY_SIZE)) == NULL) {
		if (pNewStr != NULL)
			free(pNewStr);
		return -1;
	}

	// assign new content to old pointer
	free(*pStr);
	*pStr = pNewStr;

	// check if '\0' is set
	if ((*pStr)[NEW_SIZE - 1] != '\0')
		(*pStr)[NEW_SIZE - 1] = '\0';

	// new size is now the current size
	*pOld_size = NEW_SIZE;

	return 0;
}

void freeAll(char **pText)
{
	if (*pText != NULL)
		free(*pText);
}

int main(int argc, char **argv)
{
	printf("allocation demo\n");

	char *pText = NULL;
	unsigned int text_size = 0;

	// init text
	printf("%d: init text\n", init_text(&pText, "foo bar", &text_size));
	printf("\t\"%s\", size: %d - strlen: %ld + 1\n", pText, text_size,
	       strlen(pText));
	printf("\n");

	// realloc space and append text
	unsigned int add_size = strlen(" blub ");
	printf("%d: realloc some space\n",
	       get_more_space(&pText, &text_size, add_size));
	pText = strcat(pText, " blub ");
	printf("\t\"%s\", size: %d - strlen: %ld + 1\n", pText, text_size,
	       strlen(pText));
	printf("\n");

	// trim text
	unsigned int trim_size;
	for (trim_size = strlen(pText);
	     (trim_size > 0) &&
	     ((pText[trim_size] == ' ') || (pText[trim_size] == '\0'));
	     --trim_size)
		;
	// correct counter (for loop counting prob)
	++trim_size;
	// give one more due to '\0' in trim_size
	printf("%d: trim text\n",
	       trim_char(&pText, trim_size + 1, &text_size, trim_size + 1));
	printf("\t\"%s\", size: %d - strlen: %ld + 1\n", pText, text_size,
	       strlen(pText));
	printf("\n");

	// do something with malloc()
	int *pVal = NULL;
	printf("%d: use malloc\n", init_value(&pVal, 7));
	printf("\tResult: %d\n", *pVal);
	printf("\n");

	printf("READY.\n");

	freeAll(&pText);
	return EXIT_SUCCESS;
}
