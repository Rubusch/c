/*
  usage:
                  


  zombie - if a parent creates a child, but fails to perform a
  'wait()', then an entry for the zombie child will be maintained
  indefinitely in the kernel's process table. if a large number of
  such zombie children are created, they will eventually fill the
  kernel process table, preventing the creation of new
  processes. since the zombies can't be killed by a signal, the only
  way to remove them from the system is to kill their parent (or wait
  for it to exit), at which time the zombies are adopted and waited on
  by 'init', and consequently removed from the system.

  the SIGCHLD signal is sent to a parent process whenever one of its
  children terminates. by default, this signal is ignored, but we can
  catch it by installing a signal handler.

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 554
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <libgen.h>
#include <unistd.h>

#define CMD_SIZE 200


int
main(int argc, char *argv[])
{
	char cmd[CMD_SIZE];
	pid_t child_pid;

	setbuf(stdout, NULL);

	printf("parent PID=%ld\n", (long) getpid());

	switch (child_pid = fork()) {
	case -1:
		fprintf(stderr, "fork() failed\n");
		_exit(EXIT_FAILURE);

	case 0: /* child: immediately exits to become zombie */
		printf("child (PID=%ld) exiting\n", (long) getpid());
		_exit(EXIT_SUCCESS);

	default: /* parent: - */
		sleep(3);
		snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
		system(cmd);  /* view zombie child */

		// now send the 'sure kill' signal to the zombie

		if (-1 == kill(child_pid, SIGKILL)) {
			fprintf(stderr, "kill() failed\n");
			_exit(EXIT_FAILURE);
		}

		sleep(3);

		printf("after sending SIGKILL to zombie (PID=%ld):\n", (long) child_pid);
		system(cmd);  /* view zombie child */

		fprintf(stderr, "READY.\n");
		exit(EXIT_SUCCESS);
	}

	// never reached
	exit(EXIT_FAILURE);
}
