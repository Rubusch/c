// scratchy.c
/*
  System-v Message Queue (deprected, prefer POSIX Message Queues)

  messagequeue - sender
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "../messagequeue.h"

#define ME "SCRATCHY:"
#define MESSAGE "Scratchymessage"

int main(int argc, char **argv)
{
	char message[] = MESSAGE;

	// message queue key
	fprintf(stderr, "%s set up message queue key\n", ME);
	key_t mq_key = 0;
	if (0 > (mq_key = ftok("../messagequeue.h", 'B'))) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// message queue ID
	fprintf(stderr, "%s set up message queue ID\n", ME);
	int mq_flag = IPC_CREAT | 0666;
	int mq_id = 0;
	if (0 > (mq_id = msgget(mq_key, mq_flag))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

	// init message
	fprintf(stderr, "%s init message\n", ME);
	msgbuf mq_message;
	memset(mq_message.buf, '\0', BUF_SIZE);
	mq_message.type = 1;
	strncpy(mq_message.buf, message, strlen(message));

	// send message
	fprintf(stderr, "%s send message \"%s\"\n", ME, mq_message.buf);
	if (0 > msgsnd(mq_id, &mq_message, strlen(message), IPC_NOWAIT)) {
		perror("msgsnd failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}
