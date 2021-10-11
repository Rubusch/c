// shmget.c
/*
  shared memory testing grounds - shmget
*/

#define _XOPEN_SOURCE 600

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define KEY_SIZE 8

int isnumber(const char *, const unsigned int);
void readlongnumber(unsigned long *, const unsigned int, const char *);
void readnumber(unsigned int *, const unsigned int, const char *);
void readdigit(unsigned int *, const char *);

int main(int argc, char **argv)
{
	// shm_key
	key_t shm_key;
	readlongnumber(
		(unsigned long *)&shm_key, KEY_SIZE,
		"enter a key for the shared memory with up to 7 digits:");

	// size
	unsigned int size;
	readnumber(&size, KEY_SIZE, "enter a size of shared memory");

	// shm_flag
	int shm_flag;
	{
		unsigned int tmp_flag = 0;
		do {
			tmp_flag = 0;
			fprintf(stdout,
				"select one of the following flags to shmget():\n");

			puts("1\tIPC_CREAT");
			puts("2\tIPC_EXCL");
			puts("3\towner read");
			puts("4\towner write");
			puts("5\tgroup read");
			puts("6\tgroup write");
			puts("7\tother read");
			puts("8\tother write");

			readdigit(&tmp_flag, "set the flag:");

		} while ((1 > tmp_flag) || (8 < tmp_flag));

		switch (tmp_flag) {
		case 1:
			puts("IPC_CREAT");
			shm_flag = IPC_CREAT;
			break;

		case 2:
			puts("IPC_EXCL");
			shm_flag = IPC_EXCL;
			break;

		case 3:
			fprintf(stderr, "owner read (%#8.8o)\n", 0400);
			shm_flag = 0400;
			break;

		case 4:
			fprintf(stderr, "owner write (%#8.8o)\n", 0200);
			shm_flag = 0200;
			break;

		case 5:
			fprintf(stderr, "group read (%#8.8o)\n", 040);
			shm_flag = 040;
			break;

		case 6:
			fprintf(stderr, "group write (%#8.8o)\n", 020);
			shm_flag = 020;
			break;

		case 7:
			fprintf(stderr, "other read (%#8.8o)\n", 04);
			shm_flag = 04;
			break;

		case 8:
			fprintf(stderr, "owner read (%#8.8o)\n", 02);
			shm_flag = 02;
			break;

		default:
			puts("try again");
			break;
		}
	}

	// shmget
	fprintf(stderr, "shmget(%ld, %d, %#o)\n", (long)shm_key, size,
		shm_flag);
	int shm_id;
	if (-1 == (shm_id = shmget(shm_key, size, shm_flag))) {
		perror("shmget failed");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "shmget returned %d\n", shm_id);
	}

	puts("READY.");
	exit(EXIT_SUCCESS);
}

int isnumber(const char *str, const unsigned int size)
{
	char arr[size];
	memset(arr, '\0', size);
	strncpy(arr, str, size);
	arr[size - 1] = '\0';

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

void readlongnumber(unsigned long *iVal, const unsigned int digits,
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

void readdigit(unsigned int *iChr, const char *comment)
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
	*iChr = atoi(cChr);
}
