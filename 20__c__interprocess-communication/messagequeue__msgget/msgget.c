// msgget.c
/*
  message queue testing grounds - msgget()
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

void readstring(char *, const unsigned int, const char *);
int isnumber(const char *, const unsigned int);
void readdigit(unsigned int *, const char *);


int main(int argc, char **argv)
{
	int msg_flag = 0, mq_id = 0;
	char filename[FILENAME_MAX];
	memset(filename, '\0', FILENAME_MAX);
	unsigned int iFlag = 0;

	// get a key by reading in
	readstring(filename, FILENAME_MAX, "enter a filename to generate a key:");
	key_t key = ftok(filename, 'B');

	// read in msg_flag
	do {
		readdigit(&iFlag, "flag:\n1\tIPC_CREAT\n2\tIPC_PRIVATE\n3\tIPC_"
			  "EXCL\n4\tIPC_CREAT and IPC_EXCL");

		switch (iFlag) {
		case 1:
			puts("IPC_CREAT");
			msg_flag = IPC_CREAT;
			break;

		case 2:
			puts("IPC_PRIVATE");
			msg_flag = IPC_PRIVATE;
			break;

		case 3:
			puts("IPC_EXCL");
			msg_flag = IPC_EXCL;
			break;

		case 4:
			puts("IPC_CREAT and IPC_EXCL");
			msg_flag = IPC_CREAT | IPC_EXCL;
			break;

		default:
			puts("try again");
			break;
		}
	} while (0 >= iFlag || 4 < iFlag);

	// msgget() part
	if (0 > (mq_id = msgget(key, msg_flag))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "msgget succeeded: mq_id = %d\n", mq_id);
	}

	puts("READY.");
	exit(EXIT_SUCCESS);
}


void readstring(char *cTxt, const unsigned int textSize, const char *comment)
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
		memset(cTxt, '\0', textSize);
		puts(comment);

		// read in
		unsigned int c;
		unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
			if ((textSize - 1) > idx)
				cTxt[idx] = c;
			else if ((textSize - 1) == idx) {
				puts("input too long - will be reset");
				memset(cTxt, '\0', textSize);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}

	} while (0 == strlen(cTxt));
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


// READ
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
