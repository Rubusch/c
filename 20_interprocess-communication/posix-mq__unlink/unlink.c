/*
  POSIX messagequeue - unlink

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


  The mq_unlink() function removes a posix message queue identified by
  name and marks the queue to be destroyed once all processes cease
  using it (this may mean immediately, if all processes that had the
  queue open have already closed it).


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1066
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
	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <mq name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (-1 == mq_unlink(argv[1])) {
		perror("unlink()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
