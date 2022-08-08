// message_rec.c
/*
  System-v Message Queue (deprected, prefer POSIX Message Queues)

  reads the message from the queue

  ftok -> 'B', project ID, an int

  Must be compiled with:
  -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MSG_SIZ 128

typedef struct msgbuf {
	long msg_type;
	char msg_text[MSG_SIZ];
} msgbuf;

int main(int argc, char **argv)
{
	int mq_id = 0;
	msgbuf msg_buf;

	// mq_key - get the message queue id for the name 1234 which was created by
	// the server
	key_t mq_key = 0;
	if (0 > (mq_key = ftok("message_rec.c", 'B'))) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// mq_id - connect to the queue
	if (0 > (mq_id = msgget(mq_key, 0666))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

	// msgrcv - receive an answer of message type 1
	if (0 > msgrcv(mq_id, &msg_buf, MSG_SIZ, 1, 0)) {
		perror("msgrcv failed");
		exit(EXIT_FAILURE);
	}

	// print the answer
	printf("%s\n", msg_buf.msg_text);

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
