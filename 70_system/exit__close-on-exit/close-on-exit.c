/*
  usage:
  $ ./close-on-exit.elf
    -rwxr-xr-x 1 pi pi 10632 May 16 22:40 ./close-on-exit.elf

  $ ./close-on-exit.elf x
    ls: write error: Bad file descriptor


  close-on-exit FD_CLOEXEC

  Sometimes, it may be desirable to ensure that certain file
  descriptors are closed before an exec(). In particular, if we exec()
  an unknown program (i.e., one that we did not write) from a
  privileged process, or a program that doesn't need descriptors for
  files we have already opened, then it is secure programming practice
  to ensure that all unnecessary file descriptors are closed before
  the new program is loaded.

  it is secure program practice to ensure that all unnecessary file
  descriptors are closed before the new program (exec) is loaded


  NB
  - As a general principle, library functions should always set the
    close-on-exec flag

  - If the exec() call fails for some reason, we may want to keep the
    file descriptors open. If they are already closed, it may be
    difficult, or impossible to reopen them so that they refer to the
    same files


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 576
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>

int
main(int argc, char *argv[])
{
	int flags;

	if (1 < argc) {
		flags = fcntl(STDOUT_FILENO, F_GETFD); // fetch flags
		if (-1 == flags) {
			fprintf(stderr, "fcntl() F_GETFD failed\n");
			exit(EXIT_FAILURE);
		}

		flags |= FD_CLOEXEC; // turn on FD_CLOEXEC
		if (-1 == fcntl(STDOUT_FILENO, F_SETFD, flags)) { // update flags
			fprintf(stderr, "fcntl() F_SETFD failed\n");
			exit(EXIT_FAILURE);
		}
	}

	execlp("ls", "ls", "-l", argv[0], (char*) NULL);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
