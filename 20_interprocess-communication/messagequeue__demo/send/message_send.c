// message_send.c
/*
  creates a message queue and sends one message to the queue

  - the message queue is created with a basic key and message flag
  message_flag = IPC_CREATE | 0666
  create queue and make it read and appendable by all

  - a message of type (msg_buf.message_type) 1 is sent to the queue
  with the message "can you dig it?"

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
	// 0666 - the privileges to read, write, execute..
	msgbuf msg_buf;
	size_t buf_len = 0;

	// mq_key
	key_t mq_key = 0;
	if (0 > (mq_key = ftok("../receive/message_rec.c", 'B'))) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// mq_id - get the message queue id for the "name" 1234 which was created by
	// the server
	int msg_flag = IPC_CREAT | 0666;
	fprintf(stderr, "\nmsgget: calling msgget(%#xl, %#o)\n", mq_key,
		msg_flag);
	int mq_id = 0;
	if (0 > (mq_id = msgget(mq_key, msg_flag))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "msgget: msgget succeeded: mq_id = %d\n",
			mq_id);
	}

	// msg_buf - the content, send message type 1
	msg_buf.msg_type = 1;
	fprintf(stderr, "msgget: msgget succeeded: mq_id = %d\n", mq_id);
	strncpy(msg_buf.msg_text, "can you dig it?",
		(1 + strlen("can you dig it?")));
	fprintf(stderr, "msgget: msgget succeeded: mq_id = %d\n", mq_id);
	buf_len = strlen(msg_buf.msg_text) + 1;

	// msgsnd() - send message
	if (0 > msgsnd(mq_id, &msg_buf, buf_len, IPC_NOWAIT)) {
		perror("msgsnd failed");
		exit(EXIT_FAILURE);
	} else {
		printf("message: \"%s\" sent\n", msg_buf.msg_text);
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
