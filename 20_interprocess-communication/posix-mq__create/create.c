/*
  POSIX messagequeue - create

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


  The program provides a command-line interface to the mq_open()
  function and shows how the mq_attr structure is used with
  mq_open(). Two command-line options allow message queue attributes
  to be specified: -m for mq_maxmsg and -s for mq_msgsize. If either
  of these options is supplied, a non-NULL attrp argument is passed to
  mq_open(). Some default values are assigned to the fields of the
  mq_attr structure to which attrp points, in case only one of the -m
  and -s options is specified on the command line. If neither of these
  options is supplied, attrp is specified as NULL when calling
  mq_open(), which causes the queue to be created with the
  implementation-defined defaults for the queue attributes.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1069
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h> /* link with -lrt */


#include "tlpi/get_num.h"


static void
usage(const char *name)
{
	fprintf(stderr, "usage:\n"
		"$ %s [-cx] [-m maxmsg] [-s msgsize] <mq name> [octal-perms]\n",
		name);
	fprintf(stderr, "\t-c\tcreate queue (O_CREAT)\n");
	fprintf(stderr, "\t-m maxmsg\tsetmaximum number of messages\n");
	fprintf(stderr, "\t-s msgsize\tset maximum message size\n");
	fprintf(stderr, "\t-x\tcreate exclusively (O_EXCL)\n");

	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int flags, opt;
	mode_t perms;
	mqd_t mqd;
	struct mq_attr attr, *attrp;

	attrp = NULL;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 2048;
	flags = O_RDWR;

	/*
	  parse command-line options
	*/

	while (-1 != (opt = getopt(argc, argv, "cm:s:x"))) {
		switch (opt) {
		case 'c':
			flags |= O_CREAT;
			break;

		case 'm':
			attr.mq_maxmsg = atoi(optarg); // getopt() implies having the 'optarg' available
			attrp = &attr;
			break;

		case 's':
			attr.mq_msgsize = atoi(optarg);
			attrp = &attr;
			break;

		case 'x':
			flags |= O_EXCL;
			break;

		default:
			usage(argv[0]);
		}
	}

	if (argc <= optind) {
		usage(argv[0]);
	}

	perms = (argc <= optind + 1)
		? (S_IRUSR|S_IWUSR)
		: get_int(argv[optind + 1], GN_BASE_8, "octal-perms");

	mqd = mq_open(argv[optind], flags, perms, attrp);
	if (mqd == (mqd_t) -1) {
		perror("mq_open()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
