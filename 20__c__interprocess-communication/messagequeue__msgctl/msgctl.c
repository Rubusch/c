// msgctl.c
/*
  message queue testing grounds - msgctl()

  use with a corresponding "client"
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>

#define TMP_SIZ 10
#define MQID_SIZ 10
#define FGETS_DEFAULT 3

static void do_msgctl(int, int, struct msqid_ds *);
int isnumber(const char *, const unsigned int);
void readdigit(unsigned int *, const char *);
void readstring(char *, const unsigned int, const char *);
void readnumber(unsigned int *, const unsigned int, const char *);


int main(int argc, char **argv)
{
	puts("!!! May need root permissions !!!");
	puts("msgctl program");
	int mq_cmd = 0;

	// get a key by reading in
	char filename[FILENAME_MAX];
	memset(filename, '\0', FILENAME_MAX);
	readstring(filename, FILENAME_MAX, "enter a filename to generate a key:");
	key_t key = ftok(filename, 'B');

	// msgget() part
	int mq_id = 0;
	if (0 > (mq_id = msgget(key, IPC_CREAT))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "msgget succeeded: mq_id = %d\n", mq_id);
	}

	// Linux internal struct for the messagequeue
	struct msqid_ds buf;
	unsigned int iCmd = 0;
	do {
		// reset
		iCmd = 0;
		mq_cmd = 0;
		readdigit(
			&iCmd,
			"\nenter a command:\n0\texit\n1\tIPC_SET\n2\tIPC_STAT\n3\tIPC_RMID");

		switch (iCmd) {
		case 0:
			puts("exiting");
			break;

		case 1:
			puts("IPC_SET");
			// IPC_SET - first get the content of the msqid_ds struct and show its
			// content
			mq_cmd = IPC_SET;
			fprintf(stderr, "before IPC_SET, get current values:");
			// no break here!

		case 2:
			puts("IPC_STAT");
			// get a copy of the current message queue control struture and show it to
			// the user
			do_msgctl(mq_id, IPC_STAT, &buf);
			fprintf(stderr, "msqid_ds.msg_perm.uid = %d\n", buf.msg_perm.uid);
			fprintf(stderr, "msqid_ds.msg_perm.gid = %d\n", buf.msg_perm.gid);
			fprintf(stderr, "msqid_ds.msg_perm.cuid = %d\n", buf.msg_perm.cuid);
			fprintf(stderr, "msqid_ds.msg_perm.cgid = %d\n", buf.msg_perm.cgid);
			fprintf(stderr, "msqid_ds.msg_perm.mode = %#o\n", buf.msg_perm.mode);
			fprintf(stderr, "access permissions = %#o\n", buf.msg_perm.mode & 0777);
			//	fprintf(stderr, "msqid_ds.msg_cbytes = %d\n", buf.msg_cbytes);
			//// check NA?!
			fprintf(stderr,
				"msqid_ds.msg_qbytes = %d (max number of bytes on queue)\n",
				( unsigned short )buf.msg_qbytes);
			fprintf(stderr, "msqid_ds.msg_qnum = %d\n",
				( unsigned short )buf.msg_qnum);
			fprintf(stderr, "msqid_ds.msg_lspid = %d (pid of last msgsnd)\n",
				buf.msg_lspid);
			fprintf(stderr, "msqid_ds.msg_lrpid = %d (last receive pid)\n",
				buf.msg_lrpid);
			fprintf(stderr, "msqid_ds.msg_stime = %s (last msgsnd time)\n",
				buf.msg_stime ? ctime(&buf.msg_stime) : "N/A");
			fprintf(stderr, "msqid_ds.msg_rtime = %s (last msgrcv time)\n",
				buf.msg_rtime ? ctime(&buf.msg_rtime) : "N/A");
			fprintf(stderr, "msqid_ds.msg_ctime = %s (last change time)\n",
				buf.msg_ctime ? ctime(&buf.msg_ctime) : "N/A");

			if (mq_cmd != IPC_SET) {
				// show the IPC_STAT content in case IPC_STAT was chosen originally -
				// break here
				break;
			}

			char tmp[TMP_SIZ];
			memset(tmp, '\0', TMP_SIZ);
			readnumber(&buf.msg_perm.uid, TMP_SIZ, "enter msg_perm.uid:");
			readnumber(&buf.msg_perm.gid, TMP_SIZ, "enter msg_perm.gid:");
			readnumber(( unsigned int * )&buf.msg_perm.mode, TMP_SIZ,
				   "enter msg_perm.mode:"); // FIXME
			readnumber(( unsigned int * )&buf.msg_qbytes, TMP_SIZ,
				   "enter msg_qbytes:"); // FIXME

			// perform changes
			do_msgctl(mq_id, IPC_SET, &buf);
			break;

		case 3:
			puts("IPC_RMID");
			mq_cmd = IPC_RMID;
			// reset the system
			do_msgctl(mq_id, mq_cmd, ( struct msqid_ds * )NULL);

		default:
			puts("try again\n");
			//  do nothing
			break;
		}
	} while (0 != iCmd);

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


static void do_msgctl(int mq_id, int cmd, struct msqid_ds *buf)
{
	register int rtrn; // hold area for return value from msgctl()

	fprintf(stderr, "calling msgctl(%d, %d, %s)\n", mq_id, cmd,
		buf ? "&buf" : "(struct msqid_ds*) NULL");
	if (-1 == (rtrn = msgctl(mq_id, cmd, buf))) {
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "msgctl returned %d\n", rtrn);
	}
}
