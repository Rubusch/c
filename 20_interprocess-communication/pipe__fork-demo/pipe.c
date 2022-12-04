/*
  pipe() - low level pipes
  int pipe(int fd[2])
  fd[0]    reading
  fd[1]    writing

  The main program calls pipe() to create a pipe, and then calls
  fork() to create a child. After the fork(), the parent process
  closes its file descriptor for the read end of the pipe, and writes
  the string given as the program's command-line argument to the write
  end of the pipe. The parent then closes the write end of the pipe,
  and calls wait() to wait for the child to terminate. After closing
  its file descriptor for the write end of the pipe, the child process
  enters a loop where it reads blocks of data (of up to BUF_SIZE
  bytes) from the pipe and writes them to standard output. When the
  child encounters end-of-file on the pipe, it exits the loop, writes
  a trailing newline character, closes its descriptor for the read end
  of the pipe, and terminates.

  - communication: read(), write()
  - to be closed with close(int fd)

  Takes a message as argument to send over a named pipe (or its first
  characters).


  References:
  developer manpages
  The Linux Programming Interface, Michael Kerrisk, 2010, p. 896
*/

#define _XOPEN_SOURCE 600

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int pfd[2];
	pid_t cpid;
	char buf;
	char message[64];
	memset(message, '\0', sizeof(message));

	if (2 != argc) {
		fprintf(stderr, "usage: $ %s <message to send over pipe>\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(message, argv[1], sizeof(message) - 1);
	fprintf(stdout, "message '%s'\n", message);

	// pipe
	if (0 > pipe(pfd)) {
		perror("pipe failure");
		exit(EXIT_FAILURE);
	}

	// fork
	if (0 > (cpid = fork())) {
		perror("fork failed");
		return EXIT_FAILURE;

	} else if (0 == cpid) {
		/* child code - read from pipe */

		// close unused write - idx == 1
		close(pfd[1]);

		// read out of pipe - idx == 0
		fprintf(stderr, "reading: "); // needs to be stderr (unbuffered)
		while (0 < read(pfd[0], &buf, 1)) {
			write(STDOUT_FILENO, &buf, 1);
		}
		write(STDOUT_FILENO, "\n", 1);

		// and close
		close(pfd[0]);

		fprintf(stdout, "child READY.\n");
		_exit(EXIT_SUCCESS);

	} else {
		/* parent code */

		// close unused read - idx == 0 // read
		close(pfd[0]);

		// write into pipe - idx == 1 // write
		fprintf(stdout, "writing: %s\n", message);
		write(pfd[1], message, strlen(message));

		// reader will see EOF
		close(pfd[1]);

		// wait for child
		wait(NULL);

		fprintf(stdout, "parent READY.\n");
		_exit(EXIT_SUCCESS);
	}
}
