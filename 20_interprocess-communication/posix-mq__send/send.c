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


  The program provides a command-line interface to the mq_send()
  function.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1073
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
		"$ %s [-n] <name> <message> [prio]\n", name);
	fprintf(stderr, "\t-n\tuse O_NONBLOCK flag\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int flags, opt;
	mqd_t mqd;
	unsigned prio;

	flags = O_WRONLY;
	while (-1 != (opt = getopt(argc, argv, "n"))) {
		switch (opt) {
		case 'n': flags |= O_NONBLOCK;
			break;
		default: usage(argv[0]);
		}
	}

	if (argc <= optind + 1) {
		usage(argv[0]);
	}

	mqd = mq_open(argv[optind], flags);
	if ((mqd_t) -1 == mqd) {
		perror("mq_open()");
		exit(EXIT_FAILURE);
	}

	prio = (argc > optind + 2)
		? atoi(argv[optind + 2])
		: 0;

	if (-1 == mq_send(mqd, argv[optind + 1], strlen(argv[optind + 1]), prio)) {
		perror("mq_send()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
