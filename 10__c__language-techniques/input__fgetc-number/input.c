// input_number_normal.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-May-01

  demonstrates the reading from the console, it reads a text and turns
  it into a valid integer value or continues asking
//*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXT_SIZE 10

int isnumber(const char *, const unsigned int);
void readnumber(unsigned int *, const unsigned int, const char *);

// TOOLS
int isnumber(const char *szNum, const unsigned int szNum_size)
{
	char arr[szNum_size];

	if (0 == szNum_size)
		return 0;
	memset(arr, '\0', szNum_size);
	strncpy(arr, szNum, szNum_size);
	arr[szNum_size - 1] = '\0';

	const unsigned int len = strlen(arr);
	if (0 == len)
		return 0;

	int idx;
	for (idx = 0; idx < len; ++idx) {
		if (('\n' == arr[idx]) || ('\0' == arr[idx]))
			break;
		if (!isdigit(arr[idx]))
			return 0;
	}
	return 1;
}

// READING
void readnumber(unsigned int *iVal, const unsigned int digits,
		const char *comment)
{
	if (NULL == comment) {
		perror("text is NULL");
		return;
	}
	if (NULL == iVal) {
		perror("iVal is NULL");
		return;
	}

	unsigned int size = digits + 1;
	char cTxt[size];
	do {
		// reset
		memset(cTxt, '\0', size);
		puts(comment);

		// read in
		unsigned int c;
		unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; '\n' != (c = fgetc(stdin)); ++idx) {
			if ((size - 1) > idx)
				cTxt[idx] = c;
			else if ((size - 1) == idx) {
				puts("input too long - will be reset");
				memset(cTxt, '\0', size);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}
	} while (!isnumber(cTxt, (1 + strlen(cTxt))));
	*iVal = atoi(cTxt);
}

int main(int argc, char **argv)
{
	// read in a string and transform it into a number: fgetc()
	puts("\nSTANDARD WAY TO READ IN A STRING");
	unsigned int iTxt1 = 0;
	readnumber(&iTxt1, TXT_SIZE, "enter a number up to 10 digits");
	printf("the number was '%d'\n", iTxt1);

	puts("READY.");
	exit(EXIT_SUCCESS);
}
