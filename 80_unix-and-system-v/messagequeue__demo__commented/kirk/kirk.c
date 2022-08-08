// kirk.c
/*
  System-v Message Queue (deprected, prefer POSIX Message Queues)

  Must be compiled with:
  -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MSG_SIZ 200

typedef struct {
	long msg_type;
	char msg_text[MSG_SIZ];
} msgbuf;

int main(int argc, char **argv)
{
	msgbuf msg_buf;
	int mq_id = 0;
	key_t mq_key = 0;

	// mq_key - a "valid file and a 8bit char" - just any, but has to be the same
	/*
	  The ftok() function uses the identity of the file named by
	  the given pathname (which must refer to an existing,
	  accessible file) and the least significant 8 bits of proj_id
	  (which must be nonzero) to generate a key_t type System V
	  IPC key, suitable for use with msgget(2), semget(2), or
	  shmget(2).
	 */
	if (0 > (mq_key = ftok("kirk.c", 'B'))) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// mq_id
	/*
	  The msgget() function shall return the message queue
	  identifier associated with the argument key. (XSI message
	  queues, no realtime)
	 */
	if (0 > (mq_id = msgget(mq_key, 0644 | IPC_CREAT))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

	// type - we don't really care in this case and set '1'
	msg_buf.msg_type = 1;

	// send message
	puts("enter lines of text, CTRL+\\ to quit:");
	while (1) {
		// reset
		memset(msg_buf.msg_text, '\0', MSG_SIZ);
		printf("kirk: ");

		fgets(msg_buf.msg_text, MSG_SIZ, stdin);
		if (0 > msgsnd(mq_id, (msgbuf *)&msg_buf, sizeof(msg_buf), 0)) {
			perror("msgsnd failed");
		}
	}
	/* - TESTING GROUNDS -
	   if(0 > msgctl(mq_id, IPC_RMID, NULL)){
	   perror("msgctl failed");
	   exit(EXIT_FAILURE);
	   }
	//*/
	exit(EXIT_SUCCESS);
}
