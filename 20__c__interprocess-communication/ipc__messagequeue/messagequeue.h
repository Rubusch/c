// messagequeue.h
/*
  common definitions
*/

#define BUF_SIZE 64

typedef struct msgbuf {
	long type;
	char buf[BUF_SIZE];
} msgbuf;
