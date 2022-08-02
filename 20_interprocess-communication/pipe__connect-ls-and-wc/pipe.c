/*
  usage:
  $ ./pipe.elf
    4
    READY.

  executed 'ls | wc', since 4 entries were in that directory '4' is
  displayed.


  After building a pipe, this program creates two child processes. The
  first child binds its standard output to the write end of the pipe
  and then execs ls. The second child binds its standard input to the
  read end of the pipe and then execs wc.


  References:
  The Linux Programming Interface, Michael Kerrisk, 2010, p. 900
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <unistd.h>


int
main(int argc, char *argv[])
{
	int pfd[2];

	if (-1 == pipe(pfd)) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	switch(fork()) {
	case -1:
		perror("fork()");
		exit(EXIT_FAILURE);

	case 0:  // child, execs 'ls'
		if (-1 == close(pfd[0])) {
			fprintf(stderr, "close() == 1\n");
			_exit(EXIT_FAILURE);
		}

		/*
		  duplicate stdout on write end of pipe; close
		  duplicated descriptor
		*/

		if (STDOUT_FILENO != pfd[1]) {
			if (-1 == dup2(pfd[1], STDOUT_FILENO)) { // defensive check
				fprintf(stderr, "dup2() 1\n");
				_exit(EXIT_FAILURE);
			}
			if (-1 == close(pfd[1])) {
				fprintf(stderr, "close() 2\n");
				_exit(EXIT_FAILURE);
			}

			execlp("ls", "ls", (char*) NULL); // writes to pipe
			fprintf(stderr, "execlp() ls\n");
			_exit(EXIT_FAILURE);
		}

	default:  // parent falls through to create next child
		break;
	}


	switch (fork()) {
	case -1:
		perror("fork()");
		exit(EXIT_FAILURE);

	case 0:  // second child: exec 'wc' to read from pipe
		if (-1 == close(pfd[1])) {
			fprintf(stderr, "close() 3\n");
			_exit(EXIT_FAILURE);
		}

		/*
		  duplicate stdin on read end of pipe; close
		  duplicated descriptor
		*/
		if (pfd[0] != STDIN_FILENO) {
			if (-1 == dup2(pfd[0], STDIN_FILENO)) {
				fprintf(stderr, "dup2 2\n");
				_exit(EXIT_FAILURE);
			}
			if (-1 == close(pfd[0])) {
				fprintf(stderr, "close() 4\n");
				_exit(EXIT_FAILURE);
			}
		}

		execlp("wc", "wc", "-l", (char*) NULL); // reads from pipe
		fprintf(stderr, "execlp() wc\n");
		_exit(EXIT_FAILURE);

	default:  // parend falls through
		break;
	}

	/*
	  parent closes unused file descriptors for pipe, and waits
	  for children
	*/

	if (-1 == close(pfd[0])) {
		fprintf(stderr, "close() 5\n");
		exit(EXIT_FAILURE);
	}
	if (-1 == close(pfd[1])) {
		fprintf(stderr, "close() 6\n");
		exit(EXIT_FAILURE);
	}
	if (-1 == wait(NULL)) {
		fprintf(stderr, "wait() 1\n");
		exit(EXIT_FAILURE);
	}
	if (-1 == wait(NULL)) {
		fprintf(stderr, "wait() 2\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
