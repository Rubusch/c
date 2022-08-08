/*
  POSIX messagequeue - getattributes

  usage:
  $ ./posix-mq__create/create.elf -cx /mq
    READY.

  $ ./posix-mq__getattr/getattr.elf /mq
    maximum number of messages on queue:    10
    maximum message size:                   8192
    number of messages currently on queue:  0
    READY.

  $ ./posix-mq__unlink/unlink.elf /mq
    READY.


  The program employs mq_getattr() to retrieve the attributes for the
  message queue specified in its command-line argument, and then
  displays those attributes on standard output.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1071
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h> /* link with -lrt */


int
main(int argc, char *argv[])
{
	mqd_t mqd;
	struct mq_attr attr;

	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <mq name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mqd = mq_open(argv[1], O_RDONLY);
	if ((mqd_t) -1 == mqd) {
		perror("mq_open()");
		exit(EXIT_FAILURE);
	}

	if (-1 == mq_getattr(mqd, &attr)) {
		perror("mq_getattr()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "maximum number of messages on queue:\t%ld\n", attr.mq_maxmsg);
	fprintf(stderr, "maximum message size:\t\t\t%ld\n", attr.mq_msgsize);
	fprintf(stderr, "number of messages currently on queue:\t%ld\n", attr.mq_curmsgs);


	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
