/*
  commandline input functions - implementation
*/

#include "lib_cmdline_input.h"

/*
  isdigit() checks for a digit (0 through 9).

  This wrapper checks a string if it is a number with one or more
  digits.

  #include <ctype.h>

  @strnum: The number as a string.
  @strnum_size: The size of the string representing a number.

  Returns the int representation of the number.
*/
int isnumber(const char *strnum, const unsigned int strnum_size)
{
	char arr[strnum_size];
	memset(arr, '\0', strnum_size);
	strncpy(arr, strnum, strnum_size);
	arr[strnum_size - 1] = '\0';

	const unsigned int len = strlen(arr);
	if (0 == len)
		return 0;

	register int idx;
	for (idx = 0; idx < len; ++idx) {
		if (('\n' == arr[idx]) || ('\0' == arr[idx]))
			break;
		if (!isdigit(arr[idx]))
			return 0;
	}
	return 1;
}

/*
  fgetc() reads the next character from stream and returns it as an
  unsigned char cast to an int, or EOF on end of file or error.

  This wrapper reads a string from stdin input via fgetc by a given
  size.

  #include <stdio.h>

  @str: Where the read string is stored to.
  @strsize: The size of the string to read.
  @prompt: Some text to ask for input, no terminating EOL is needed.
*/
void readstring(char *str, const unsigned int strsize, const char *prompt)
{
	if (NULL == prompt) {
		perror("text is NULL");
		return;
	}
	if (NULL == str) {
		perror("iTxt is NULL");
		return;
	}

	do {
		// reset
		memset(str, '\0', strsize);
		fprintf(stdout, "%s\n", prompt);

		// read in
		register unsigned int c;
		register unsigned int idx = 0;

		// in case of exceeding the size of the variable - put
		// a '\0' at the end and read until '\n', but don't
		// store the characters for cleaning the stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
			if ((strsize - 1) > idx) {
				// append to string
				str[idx] = c;
			} else if ((strsize - 1) == idx) {
				/* // either: input too long, reset all
				fprintf(stdout, "input too long - will be reset\n");
				memset(str, '\0', strsize);
				/*/ // or: terminate string
				str[idx] = '\0';
				// */
			}
		}

	} while (0 == strlen(str));
}

/*
  TODO   
*/
void readdigit(unsigned int *iVal, const char *comment)
{
	char cChr[3];
	do {
		// reset
		memset(cChr, '\0', 3);
		fprintf(stdout, "%s\n", comment);

		// read
		fgets(cChr, 3, stdin);

		// check stdin and clean in case input exceeded the
		if ('\n' == cChr[1]) {
			// ok - do nothing here

		} else if (('\n' == cChr[0]) || ('\n' == cChr[2])) {
			// number too long or input empty - erease
			memset(cChr, '\0', 3);

		} else {
			// '\n' still in stdin - clean up
			fprintf(stdout, "input too long - will reset\n");
			memset(cChr, '\0', 3);
			while ('\n' != fgetc(stdin))
				;
		}

		// CAUTION: due to '\n' checking the set of '\0' here!
		cChr[2] = '\0';

		// number?
	} while (!isdigit(cChr[0]));

	// transform
	*iVal = atoi(cChr);
}

/*
  TODO
*/
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
		fprintf(stdout, "%s\n", comment);

		// read in
		register unsigned int c;
		register unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
			if ((size - 1) > idx)
				cTxt[idx] = c;
			else if ((size - 1) == idx) {
				fprintf(stdout,
					"input too long - will be reset\n");
				memset(cTxt, '\0', size);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}
	} while (!isnumber(cTxt, (1 + strlen(cTxt))));
	*iVal = atoi(cTxt);
}

/*
  TODO
*/
void readlongnumber(unsigned long int *iVal, const unsigned int digits,
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
		fprintf(stdout, "%s\n", comment);

		// read in
		register unsigned int c;
		register unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
			if ((size - 1) > idx)
				cTxt[idx] = c;
			else if ((size - 1) == idx) {
				fprintf(stdout,
					"input too long - will be reset\n");
				memset(cTxt, '\0', size);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}
	} while (!isnumber(cTxt, (1 + strlen(cTxt))));
	*iVal = atol(cTxt);
}
