// mq_sample.c
/*
  demonstrates msgsnd() and msgrcv() and works more or less - this is
  experimental grounds!!!

  <sys/msg.h> includes a definition of struct msgbuf with the mtext field
  defined as: char mtext[1]

  therefore, this definition is only a template, not a structure definition that
  you can use directly, unless you want only to send and receive messages of 0 or
  1 byte. To handle this, malloc an area big enough to obtain the template - the
  size of the mtext template field + the size of the mtext field wanted. Then you
  can use the pointer returned by malloc as a struct msgbuf with an mtext field of
  the size you want. Note also that sizeof(msg_ptr->mtext is valid even though
  msg_ptr isn't pointing to anything yet. Sizeof doesn't de-reference msg_ptr, but
  uses its type to figure out what you are asking about.
*/

#define _XOPEN_SOURCE 600

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define TMP_SIZE 8
#define MSG_TYPE 34L
#define MSG_SIZE_MAX 32

#ifndef msgbuf
typedef struct msgbuf {
	long mtype;
	char mtext[16];
} msgbuf;
#endif


static int ask();
char first_on_queue[] = "->first message on queue",
	full_buf[] = "Message buffer overflow. Extra message text discarded.";
int isnumber(const char *, const unsigned int);
void readnumber(unsigned int *, const unsigned int, const char *);
void readstring(char *, const unsigned int, const char *);
void readdigit(int *, const char *);


int main(int argc, char **argv)
{
	char tmp[TMP_SIZE];

	// user's selected operation code
	int choice;

	// message flags for the operation
	int msg_flag;

	// message size
	int msg_size;

	// desired message type
	long msg_type;

	// message queue ID to be used
	int mq_id;

	// size of allocated message buffer
	int max_msg_size;

	// return value from msgrcv() or msgsnd()
	int res;

	/*
	  build a messagequeue
	//*/
	puts("build messagequeue");

	// mq key
	key_t key = 0;
	if (0 > (key = ftok("mq_sample.c", 'B'))) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// mq id
	if (0 > (mq_id = msgget(key, IPC_CREAT | 0666))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

	// max buffer size
	max_msg_size = MSG_SIZE_MAX;
	/*
	  readnumber(&max_msg_size, TMP_SIZE, "enter the message buffer size you
	  want:");
	//*/

	// pointer to the message buffer
	struct msgbuf *msg_ptr;
	if (NULL == (msg_ptr = malloc(sizeof(*msg_ptr)))) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	while (0 != (choice = ask())) {
		switch (choice) {
		case 1:
			// message type
			msg_type = MSG_TYPE;
			/* don't ask
			   readnumber(&tmp, TMP_SIZE, "enter msg_ptr->mtype:");
			//*/

			// read in message
			if (max_msg_size) {
				fprintf(stderr, "enter a one line message\n");

				int idx = 0;
				register int c = 0;
				for (idx = 0; ('\n' != (c = getchar())); ++idx) {
					if (idx >= max_msg_size) {
						fprintf(stderr, "\n%s\n", full_buf);
						while ('\n' != (c = getchar()))
							;
						break;
					}
					msg_ptr->mtext[idx] = c;
				}
				msg_size = idx;
				msg_ptr->mtype = msg_type; // just set any number >0
			} else {
				msg_size = 0;
				msg_ptr->mtype = 0; // msgsnd() won't deliver if <=0
			}
			// read in flag
			fprintf(stderr, "\available msgsnd flags:\n");
			fprintf(stderr, "\tIPC_NOWAIT = \t%#8.8o\n", IPC_NOWAIT);

			{ // set IPC_NOWAIT flag
				int repeat = 1;
				do {
					memset(tmp, '\0', TMP_SIZE);
					readstring(tmp, TMP_SIZE, "enter msgflg (y/n): ");
					if (0 < strlen(tmp)) {
						if ('y' == tmp[0] || 'Y' == tmp[0]) {
							msg_flag = IPC_NOWAIT;
							repeat = 0;
						} else if ('n' == tmp[0] || 'N' == tmp[0]) {
							msg_flag = 0;
							repeat = 0;
						} else {
							repeat = 1;
						}
					}
				} while (repeat);
			}

			// summary
			fprintf(stderr, "summary\n");
			fprintf(stderr, "%s(%d, msg_ptr, %d, %#o)\n", "msgop: calling msgsnd",
				mq_id, msg_size, msg_flag);
			fprintf(stderr, "msg_ptr->mtype = %ld\n", msg_ptr->mtype);
			fprintf(stderr, "msg_ptr->mtext = \"");

			{ // send messages
				int idx = 0;
				for (idx = 0; idx < msg_size; ++idx)
					fputc(msg_ptr->mtext[idx], stderr);
				fprintf(stderr, "\"\n");
			}
			msg_size = sizeof(*msg_ptr);
			// send
			if (0 > (res = msgsnd(mq_id, (void*) msg_ptr, msg_size, msg_flag)))
				perror("msgsnd failed");
			else
				fprintf(stderr, "msgop: msgsnd returned %d\n", res);
			break;

		case 2:
			msg_type = MSG_TYPE;

			// read in read flag
			fprintf(stderr, "meaningful msgrcv flags are:\n");
			fprintf(stderr, "\t1. MSG_NOERROR = \t%#8.8o\n", MSG_NOERROR);
			fprintf(stderr, "\t2. IPC_NOWAIT = \t%#8.8o\n", IPC_NOWAIT);
			{
				int repeat = 1;
				do {
					int flag = 0;
					readdigit(&flag, "enter msg_flag (1 or 2): ");

					switch (flag) {
					case 1:
						msg_flag = MSG_NOERROR;
						repeat = 0;
						break;

					case 2:
						msg_flag = IPC_NOWAIT;
						repeat = 0;
						break;

					default:
						repeat = 1;
						break;
					}
				} while (repeat);
				msg_flag = atoi(tmp);
			}
			// summary
			msg_size = sizeof(*msg_ptr);
			fprintf(stderr, "msgop: calling msgrcv(%d, msg_ptr, %d, %ld, %#o);\n",
				mq_id, msg_size, msg_type, msg_flag);

			// receive
			if (-1 == (res = msgrcv(mq_id, msg_ptr, msg_size, msg_type, msg_flag))) {
				perror("msgop: msgrcv failed");
			} else {
				fprintf(stderr, "msgop: msgrcv returned %d\n", res);
				fprintf(stderr, "msg_ptr->mtype = %ld\n", msg_ptr->mtype);
				fprintf(stderr, "msg_ptr->mtext = \"");
				int idx = 0;
				for (idx = 0; idx < res; ++idx)
					fputc(msg_ptr->mtext[idx], stderr);
				fprintf(stderr, "\"\n");
			}
			break;

		default:
			puts("msgop: operation unknown\n");
			break;
		}
	}

	puts("READY.");
	free(msg_ptr);
	exit(EXIT_SUCCESS);
}


// ask
static int ask()
{
	int response = 0;
	puts("\nyour options are:");
	do {
		response = 0;
		puts("\texit = \t0 or CTRL+c");
		puts("\tmsgsnd = \t1");
		puts("\tmsgrcv = \t2\n");
		readdigit(&response, "enter your choice (0/1/2):");
	} while ((0 > response) || (2 < response));

	return response;
}


// TOOLS
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


// READING
void readnumber(unsigned int *iTxt, const unsigned int digits,
                const char *comment)
{
	if (NULL == comment) {
		perror("text is NULL");
		return;
	}
	if (NULL == iTxt) {
		perror("iTxt is NULL");
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
	*iTxt = atoi(cTxt);
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


void readdigit(int *iChr, const char *comment)
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
