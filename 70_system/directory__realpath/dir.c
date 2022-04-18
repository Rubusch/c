/*
  usage:
  $ ./dir.elf <symlink>
  e.g.
  $ ln -s ../../.circleci/ xxx

  $ ./dir.elf ./xxx
    readlink: ./xxx --> ../../.circleci/
    READY.


  demostrates the usage of realpath() on symbolic links


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 369
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <limits.h> /* PATH_MAX, max path size */

#define BUF_SIZE PATH_MAX

int
main(int argc, char *argv[])
{
	struct stat statbuf;
	char buf[BUF_SIZE];
	ssize_t nbytes;

	if (argc == 1) {
		fprintf(stderr, "usage:\n");
		fprintf(stderr, "$ %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (-1 == lstat(argv[1], &statbuf)) {
		fprintf(stderr, "lstat() failed\n");
		exit(EXIT_FAILURE);
	}

	if (!S_ISLNK(statbuf.st_mode)) {
		fprintf(stderr, "%s is not a symbolic link\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	nbytes = readlink(argv[1], buf, BUF_SIZE -1);
	if (-1 == nbytes) {
		fprintf(stderr, "readlink() failed\n");
		exit(EXIT_FAILURE);
	}

	buf[nbytes] = '\0';
	fprintf(stderr, "readlink: %s --> %s\n", argv[1], buf);

	if (NULL == realpath(argv[1], buf)) {
		fprintf(stderr, "realpath() failed\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
