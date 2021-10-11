// wait.c - part 1
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-September-27

  #include <sys/wait.h>
  pid_t wait(int* status);

  Wait for child process termination. Makes a parent wait until a
  child process is completed. BLOCKS calling process until the child
  terminates. If the parent has various child processes, wait()
  returns when one returns.

  NB: no call to free(), cleaned up by exit()

  status: the termination status of the process, can be NULL.
  Returns pid of terminated child on success, else -1
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PARENT_TXT "parent - "
#define CHILD_TXT "\tchild - "
#define IDENTIFIER_SIZE 10

int main()
{
	char *identifier = NULL;
	if (NULL ==
	    (identifier = calloc(IDENTIFIER_SIZE, sizeof(*identifier)))) {
		perror("calloc() failed");
		exit(EXIT_FAILURE);
	}
	memset(identifier, '\0', IDENTIFIER_SIZE);
	pid_t pid = 0, pid_parent = getpid();

	// fork
	puts("fork()");
	if (0 > (pid = fork())) {
		perror("fork() failed");
		exit(EXIT_FAILURE);

	} else if (pid == 0) {
		// child process
		strncpy(identifier, CHILD_TXT, 1 + strlen(CHILD_TXT));

		// child code
		printf("%schild pid: %i, parent: %i\r\n", identifier, getpid(),
		       pid_parent);
		printf("%ssleeps\r\n", identifier);
		sleep(5);
		printf("%sawakes\r\n", identifier);
		printf("%sdone\r\n", identifier);
		exit(EXIT_SUCCESS);

	} else {
		// parent process
		strncpy(identifier, PARENT_TXT, 1 + strlen(PARENT_TXT));

		printf("%schild pid: %i, parent: %i\r\n", identifier, pid,
		       getpid());
		printf("%swaiting on pid %i\r\n", identifier, pid);

		/*
		  1. child exit status
		 */
		int childExitStatus = 0;

		/*
		  2. wait on all childs
		 */
		while (0 == wait(&childExitStatus))
			;
		printf("%swaiting done\r\n", identifier);

		printf("%sdone\r\n", identifier);
		exit(EXIT_SUCCESS);
	}
}
