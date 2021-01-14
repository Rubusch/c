// spock.c
/*
  works "serverlike" and waits for input

  Must be compiled with:
  -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MSG_SIZ 200

typedef struct {
	long msg_type;
	char msg_text[MSG_SIZ];
} msgbuf;


int main(int argc, char **argv)
{
	msgbuf msg_buf;
	int mq_id = 0;
	key_t mq_key;

	// mq_key
	if (0 > (mq_key = ftok("../kirk/kirk.c", 'B'))) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// mq_id
	if (0 > (mq_id = msgget(mq_key, 0644))) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

	puts("spock: ready to receive messages, captain.");

	// receive message
	char tmp[MSG_SIZ];
	while (1) {
		// reset
		memset(tmp, '\0', MSG_SIZ);

		// receive
		if (0 > msgrcv(mq_id, ( msgbuf * )&msg_buf, sizeof(msg_buf), 0, 0)) {
			perror("msgrcv failed");
			exit(EXIT_FAILURE);
		}
		strncpy(tmp, msg_buf.msg_text, strlen(msg_buf.msg_text));
		printf("spock: #%s#\n", tmp); // remember the '\n' at the end!!!
	}
	exit(EXIT_SUCCESS);
}
