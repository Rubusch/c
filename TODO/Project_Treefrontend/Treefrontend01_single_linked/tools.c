// tools.c
/*
  console input functions - implementation
//*/

#include "tools.h"

int isnumber(const char *szNum, const unsigned int szNum_size)
{
	char arr[szNum_size];
	memset(arr, '\0', szNum_size);
	strncpy(arr, szNum, szNum_size);
	arr[szNum_size - 1] = '\0';

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

void readstring(char *cTxt, const unsigned int txtSize, const char *comment)
{
	if (NULL == comment) {
		perror("text is NULL");
		return;
	}
	if (NULL == cTxt) {
		perror("iTxt is NULL");
		return;
	}

	do {
		// reset
		memset(cTxt, '\0', txtSize);
		puts(comment);

		// read in
		register unsigned int c;
		register unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
			if ((txtSize - 1) > idx)
				cTxt[idx] = c;
			else if ((txtSize - 1) == idx) {
				puts("input too long - will be reset");
				memset(cTxt, '\0', txtSize);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}

	} while (0 == strlen(cTxt));
}

void readdigit(unsigned int *iVal, const char *comment)
{
	char cChr[3];
	do {
		// reset
		memset(cChr, '\0', 3);
		puts(comment);

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
			puts("input too long - will reset");
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
		register unsigned int c;
		register unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
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
		puts(comment);

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
				puts("input too long - will be reset");
				memset(cTxt, '\0', size);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}
	} while (!isnumber(cTxt, (1 + strlen(cTxt))));
	*iVal = atol(cTxt);
}

/*
  File operations
//*/

/*
  get the file pointer
//*/
int get_write_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
	if (*fp != NULL)
		return -1;
	if (filename == NULL)
		return -1;

	if ((*fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr,
			"fo::get_write_file_pointer(FILE**, char[]) - Failed!\n");
		return -1;
	}

	return 0;
}

/*
  write linewise
//*/
int write_linewise(FILE *fp, char *content,
		   const unsigned long int CONTENT_SIZE)
{
	if (fp == NULL)
		return -1;
	if (content == NULL)
		return -1;

	char bufLine[BUFSIZ];
	int idxLine = 0;
	int idxContent = 0;
	char *pData = &content[0];
	strcpy(bufLine, "");

	while ((idxLine < BUFSIZ) && (idxContent < CONTENT_SIZE) &&
	       ((bufLine[idxLine] = *(pData++)) != '\0')) {
		if (idxLine >= BUFSIZ) {
			fprintf(stderr,
				"fo::write_linewise(FILE*, char*, const unsigned long) - Failed!\n");
			return -1;
		}

		if (((idxLine == CONTENT_SIZE - 2) &&
		     (bufLine[idxLine] != '\n')) ||
		    (*(pData + 1) == '\0')) {
			bufLine[idxLine + 1] = '\0';
			fputs(bufLine, fp); // write line
			break;

		} else if (bufLine[idxLine] == '\n') {
			fputs(bufLine, fp); // write line
			idxLine = 0;
		} else {
			++idxLine;
		}
		++idxContent;
	}
	fputs("\n\0", fp); // tailing linefeed + linebreak
	return 0;
}

/*
  close the file pointer
//*/
int close_stream(FILE **fp)
{
	if (*fp == NULL)
		return -1;
	int iRes = fclose(*fp);
	*fp = NULL;

	return iRes;
}

/*
  data_to_string()

  transform a data value to string "element",
  add leading and tailing ' '
  if the int doesn't fit the size append ' '

  the space of element needs to be allocated before!
//*/
int data_to_string(char *str, const unsigned int str_size,
		   const unsigned int data)
{
	//  puts("\tdata_to_string()");

	// leading
	strncat(str, " ", 1);

	// convert data
	register unsigned int num = 0;
	if (0 > (num = sprintf((++str), "%3d", data))) {
		perror("ERROR: data_to_string() failed somehow");
		return -1;
	} else if (num < DATA_DIGITS) {
		register unsigned int idx = 0;
		for (idx = num; idx < DATA_DIGITS; ++idx) {
			strncat(str, " ", 1);
		}
	}

	// tailing ' '
	strncat(str, " ", 1);

	return 0;
}
