// client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>   // ftok(), msgget(), msgsnd()
#include <sys/msg.h>   // msgget(), msgsnd(), msgctl()
#include <sys/types.h> // ftok(), msgget(), msgsnd()
#include <unistd.h>    // sleep()

#include "../mq_message.h"
#define TEXT "Viva!"
typedef enum error_code_e { ERRCODE_SUCCESS, ERRCODE_FAILURE } error_code_t;


static error_code_t _mq_init(int *mq_id, char seed)
{
	key_t mq_key = 0;
	int mq_flags = -1;
	struct msqid_ds buf;

	if (!mq_id)
		return ERRCODE_FAILURE;

	if (0 > (mq_key = ftok("/tmp", seed))) {
		perror("ftok() failed");
		return ERRCODE_FAILURE;
	}

	mq_flags = IPC_CREAT | 0666;
	*mq_id = -1;
	if (0 > (*mq_id = msgget(mq_key, mq_flags))) {
		perror("msgget() failed");
		return ERRCODE_FAILURE;
	}

	msgctl(*mq_id, IPC_STAT, &buf);
	buf.msg_qbytes = MQ_MAXSIZE;
	if (0 > msgctl(*mq_id, IPC_SET, &buf)) {
		perror("msgctl() failed, try the following on a shell:");
		fprintf(stderr, "\t\tsudo sh -c \"echo %d > /proc/sys/kernel/msgmnb\"\n",
			MQ_MAXSIZE);
	}

#ifdef _GNU_SOURCE
	struct msqid_ds mq_stat;
	struct msginfo mq_info;

	puts("***************************************************");
	puts("message queue:");
	puts("IPC_STAT");
	if (-1 == msgctl(*mq_id, IPC_STAT, &mq_stat)) {
		perror("msgctl(), IPC_STAT failed");
		return;
	}
	fprintf(stderr, "\tmsg_qnum\t= %d, Current number of messages in queue\n",
		(( int )mq_stat.msg_qnum));
	fprintf(stderr,
		"\tmsg_qbytes\t= %d, Maximum number of bytes allowed in queue\n",
		(( int )mq_stat.msg_qbytes));
	fprintf(stderr, "\tsizeof( struct mq_message_s )\t= %ld\n",
		( long )sizeof(struct mq_message_s));
	fprintf(stderr, "\t-> maximum number of messages possible: %d\n",
		( int )((( int )mq_stat.msg_qbytes) / sizeof(struct mq_message_s)));
	puts("");
	puts("IPC_INFO");
	if (0 > msgctl(*mq_id, IPC_INFO, ( struct msqid_ds * )&mq_info)) {
		perror("msgctl(), IPC_INFO failed");
		return;
	}
	fprintf(stderr,
		"\tmsgmax\t\t= %d, Max. # of bytes that can be written in a single "
		"message\n",
		mq_info.msgmax);
	fprintf(stderr,
		"\tmsgmnb\t\t= %d, Max. # of bytes that can be written to queue; "
		"used to initialize msg_qbytes during queue creation (msgget())\n",
		mq_info.msgmnb);
	fprintf(stderr, "\tmsgmni\t\t= %d, Max. # of message queues\n",
		mq_info.msgmni);
	puts("***************************************************");
#endif

	return ERRCODE_SUCCESS;
}


static error_code_t _mq_send(const int mq_id, struct mq_message_s *mq_message,
                             const int mtype, const int blocking)
{
	if ((mtype != 1) && (mtype != 3)) {
		perror("mtype is neither 1, nor 3, sending message to external program "
		       "aborted");
		return ERRCODE_FAILURE;
	}

	mq_message->mtype = mtype;
	if (0 > msgsnd(mq_id, mq_message, sizeof(*mq_message), blocking)) {
		return ERRCODE_FAILURE;
	}

	return ERRCODE_SUCCESS;
}


static error_code_t _mq_retrieve(const int mq_id,
                                 struct mq_message_s *mq_message,
                                 const int mtype, const int blocking)
{
	if (!mq_message)
		return ERRCODE_FAILURE;
	mq_message->mtype = mtype;

	if ((mq_message->mtype != 2) && (mq_message->mtype != 4)) {
		perror("mtype is neither 2, nor 4, sending message to external program "
		       "aborted");
		return ERRCODE_FAILURE;
	}

	memset(mq_message->content, '\0', MQ_MESSAGESIZ);

	if (0 > msgrcv(mq_id, mq_message, sizeof(*mq_message), mtype, blocking)) {
		return ERRCODE_FAILURE;
	}

	return ERRCODE_SUCCESS;
}


// mtype:
// 1 - client -> server
// 2 - server -> client
int main(int argc, char **argv)
{
	int idx = 0;
	int mq_id = -1;
	char seed = 'A';
	static struct mq_message_s mq_message;

	// init
	memset(mq_message.content, '\0', MQ_MESSAGESIZ);
	memset(mq_message.content_name, '\0', MQ_NAMESIZE);

	strcpy(mq_message.content_name, "test");
	mq_message.content_type = MQDATA_STRING;
	mq_message.needs_confirmation = 0;
	mq_message.arr_extent = strlen(TEXT) + 1;
	strcpy(mq_message.content, TEXT);

	// init
	if (ERRCODE_SUCCESS != _mq_init(&mq_id, seed)) {
		perror("_mq_init()");
		exit(ERRCODE_FAILURE);
	}

	do {
		++idx;

		// send hello
		fprintf(stderr, "send \"%s\"\n", mq_message.content);
		mq_message.mtype = 1;
		if (ERRCODE_SUCCESS != _mq_send(mq_id, &mq_message, mq_message.mtype, 0)) {
			perror("_mq_send() failed");
		}
		sleep(1);

		// receive hello
		mq_message.mtype = 2;
		memset(mq_message.content, '\0', MQ_MESSAGESIZ);
		fprintf(stderr, "cleared \"%s\"\n", mq_message.content);
		_mq_retrieve(mq_id, &mq_message, mq_message.mtype, 0);
		fprintf(
			stderr, "received \"%s\"\n",
			((strlen(mq_message.content) > 0) ? mq_message.content : "nothing"));

	} while (3 > idx);


	exit(EXIT_SUCCESS);
}
