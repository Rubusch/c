/*
  usage:
    $ ./fork.elf
      file offset before fork(): 0
      O_APPEND flag before fork() is: off
      child has exited
      file offset in parent: 1000
      O_APPEND flag in parent is: on
      READY.


  this program opens a temporary file using mkstemp(), and then calls
  fork() to create a child process. the child changes the file offset
  and open file status flags of the temporary file, and exits. the
  parent then retrieves the file offset and flags to verify that it
  can see the changes made by the child.


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 518
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int fd, flags;
	char template[] = "/tmp/forkXXXXXX";

	setbuf(stdout, NULL);

	fd = mkstemp(template);
	if (-1 == fd) {
		perror("mkstemp() failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "file offset before fork(): %lld\n",
		(long long) lseek(fd, 0, SEEK_CUR));

	flags = fcntl(fd, F_GETFL);
	if (-1 == flags) {
		perror("fcntl() failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "O_APPEND flag before fork() is: %s\n",
		(flags & O_APPEND) ? "on" : "off");

	switch (fork()) {
	case -1:
		fprintf(stderr, "fork() failed\n");
		exit(EXIT_FAILURE);

	case 0: // child: change file offset and status flags
		if (-1 == lseek(fd, 1000, SEEK_SET)) {
			fprintf(stderr, "lseek() failed\n");
			_exit(EXIT_FAILURE);
		}

		flags = fcntl(fd, F_GETFL);  // fetch current flags
		if (-1 == flags) {
			fprintf(stderr, "fcntl() - F_GETFL\n");
			_exit(EXIT_FAILURE);
		}

		flags |= O_APPEND;  // tun O_APPEND on
		if (-1 == fcntl(fd, F_SETFL, flags)) {
			fprintf(stderr, "fcntl - F_SETFL\n");
			_exit(EXIT_FAILURE);
		}

		_exit(EXIT_SUCCESS);

	default: // parent: can see file changes made by child
		if (-1 == wait(NULL)) {
			fprintf(stderr, "wait() failed\n");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "child has exited\n");

		fprintf(stderr, "file offset in parent: %lld\n",
			(long long) lseek(fd, 0, SEEK_CUR));

		flags = fcntl(fd, F_GETFL);
		if (-1 == flags) {
			fprintf(stderr, "fcntl() failed\n");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "O_APPEND flag in parent is: %s\n",
			(flags & O_APPEND) ? "on" : "off");

		fprintf(stderr, "READY.\n");
		exit(EXIT_SUCCESS);
	}

	// never reaches to here...
	exit(EXIT_SUCCESS);
}
