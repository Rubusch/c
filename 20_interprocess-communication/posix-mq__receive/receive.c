/*
  POSIX messagequeue - send

  usage:
  $ ./posix-mq__create/create.elf -cx /mq
    READY.

  $ ./posix-mq__send/send.elf /mq msg-a 5
    READY.

  $ ./posix-mq__send/send.elf /mq msg-b 0
    READY.

  $ ./posix-mq__send/send.elf /mq msg-c 10
    READY.

  $ ./posix-mq__receive/receive.elf /mq
    read 5 bytes; priority = 10
    msg-c
    READY.

  $ ./posix-mq__receive/receive.elf /mq
    read 5 bytes; priority = 5
    msg-a
    READY.

  $ ./posix-mq__receive/receive.elf /mq
    read 5 bytes; priority = 0
    msg-b
    READY.

  $ ./posix-mq__unlink/unlink.elf /mq
    READY.


  The mq_receive() function removes the oldest message with the
  highest priority from the message queue referred to by mqdes and
  returns that message in the buffer pointed to by msg_ptr.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1076
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h> /* link with -lrt */


static void
usage(const char* name)
{
	fprintf(stderr, "usage:\n"
		"$ %s [-n] <name>\n", name);
	fprintf(stderr, "\t-n\tuse O_NONBLOCK flag\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int flags, opt;
	mqd_t mqd;
	unsigned prio;
	void *buffer;
	struct mq_attr attr;
	ssize_t num_read;

	flags = O_RDONLY;
	while (-1 != (opt = getopt(argc, argv, "n"))) {
		switch (opt) {
		case 'n': flags |= O_NONBLOCK;
			break;
		default: usage(argv[0]);
		}
	}

	if (argc <= optind) {
		usage(argv[0]);
	}

	mqd = mq_open(argv[optind], flags);
	if ((mqd_t) -1 == mqd) {
		perror("mq_open()");
		exit(EXIT_FAILURE);
	}

	if (-1 == mq_getattr(mqd, &attr)) {
		perror("mq_getattr()");
		exit(EXIT_FAILURE);
	}

	buffer = malloc(attr.mq_msgsize);
	if (NULL == buffer) {
		perror("malloc()");
		exit(EXIT_FAILURE);
	}

	num_read = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
	if (-1 == num_read) {
		perror("mq_receive()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "read %ld bytes; priority = %u\n", (long) num_read, prio);
	if (-1 == write(STDOUT_FILENO, buffer, num_read)) {
		perror("write()");
		exit(EXIT_FAILURE);
	}
	write(STDOUT_FILENO, "\n", 1);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
