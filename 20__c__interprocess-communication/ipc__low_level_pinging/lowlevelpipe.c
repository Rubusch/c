// itchy.c
/*
  low level piping - pipe()

  #include <unistd.h>
  int pipe(int fildes[2]);

  int pipe(int fd[2])
  fd[0]    reading
  fd[1]    writing

   - communication: read(), write() and only used at fork()
   - to be closed with close(int fd)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 64


void childcode(int *pfd)
{
	char ME[] = "ITCHY:";
	char buf[BUF_SIZE];


	fprintf(stderr, "%s started\n", ME);

	sleep(1); // no sync

	// close - writing
	close(pfd[1]);

	// receive
	while (0 < read(pfd[0], buf, BUF_SIZE))
		;
	fprintf(stderr, "%s received \"%s\"\n", ME, buf);

	// close - reading
	close(pfd[0]);

	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}


void parentcode(int *pfd)
{
	char ME[] = "SCRATCHY:";
	char MESSAGE[] = "Scratchymessage";

	fprintf(stderr, "%s started\n", ME);

	// close - reading
	close(pfd[0]);

	// send
	fprintf(stdout, "%s sends \"%s\"..\n", ME, MESSAGE);
	write(pfd[1], MESSAGE, 1 + strlen(MESSAGE));

	// close - writing
	close(pfd[1]);

	sleep(1); // just some delay to quit - because of the display
	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}


int main(int argc, char **argv)
{
	int pfd[2];
	pid_t pid;

	// create the pipe before forking
	if (0 > pipe(pfd)) {
		perror("pipe failed");
		exit(EXIT_FAILURE);
	}

	// fork
	fprintf(stderr, "fork()\n");
	if (0 > (pid = fork())) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		childcode(pfd);
	} else {
		parentcode(pfd);
	}

	fprintf(stderr, "never executed code\n");
	exit(EXIT_SUCCESS);
}
