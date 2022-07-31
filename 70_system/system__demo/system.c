// system.c
/*
  usage:
  $ ./system.elf
  command: ls -l
      total 28
      -rw-r--r-- 1 pi pi   298 May 17 20:00 Makefile
      -rw-r--r-- 1 pi pi  3518 May 17 20:16 system.c
      -rwxr-xr-x 1 pi pi 11172 May 17 20:16 system.elf
      -rw-r--r-- 1 pi pi  5924 May 17 20:16 system.o
      system() returned: status=0x0000 (0,0)
      child exited, status=0

  command: whoami
      pi
      system() returned: status=0x0000 (0,0)
      child exited, status=0

  command: ^C


  Executing shell commands with system()

  #include <stdlib.h>
  int system(const char* command);
  // executes a console command

  Avoid using system() in set-user-ID and set-group-ID
  programs. Set-user-ID and set-group-ID programs should never use
  system() while operating under the program's privileged
  identifier. Even when such programs don't allow the user to specify
  the text of the command to be executed, the shel's reliance on
  various environment variables to control its operation means that
  the use of system() inevitably opens the door for a system security
  breach.

  Secure programs that need to spawn another program should use fork()
  and one of the exec() functions - other than execlp() or execvp() -
  directly

  system() calls and args might make the software non-portable.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 581
*/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <stdbool.h>

#define MAX_CMD_LEN 200

/*
  NOTE: the following function employs printf(), which is not
  async-signal-safe. as such this functio is also not
  async-signal-safe

  examine a wait() status using the W* macros.
 */
void
print_wait_status(const char* msg, int status)
{
	if (NULL != msg) {
		fprintf(stderr, "%s", msg);
	}

	if (WIFEXITED(status)) {
/*
  WIFEXITED()    evaluates to a non-zero value if "status" was returned for a
                 child process that terminated normally

		 this macro returns true if the child process exited normally
*/
		fprintf(stderr, "child exited, status=%d\n", WEXITSTATUS(status));
/*
  WEXITSTATUS()  if the value is non-zero, this macro evaluates to the low-order
                 8 bits of "status" that the child process passed to _exit() or
                 exit(), or the value the child pid returned from main()
*/
	} else if (WIFSIGNALED(status)) {
/*
  WIFSIGNALED()  this macro returns true if the child process was
                 killed by a signal.
*/
		fprintf(stderr, "child killed by signal %d (%s)\n",
			WTERMSIG(status), strsignal(WTERMSIG(status)));
/*
  WTERMSIG()     if WIFSIGNALED is non-zero, this macro evaluates to the number
                 of the signal that caused the termination of the child pid
*/

#ifdef WCOREDUMP
		if (WCOREDUMP(status)) {
/*

*/
			fprintf(stderr, " (core dumped)");
		}
#endif
		fprintf(stderr, "\n");
	} else if (WIFSTOPPED(status)) {
		fprintf(stderr, "child stopped by signal %d (%s)\n",
			WSTOPSIG(status), strsignal(WSTOPSIG(status)));

#ifdef WIFCONTINUED
	} else if (WIFCONTINUED(status)) {
/*
      WIFCONTINUED this macro returns true of the child was resumed by
		 delivery of SIGCONT
*/
		fprintf(stderr, "child continued\n");
#endif
	} else { // should never happen
		fprintf(stderr, "what happened to this child? (status=%x)\n",
			(unsigned int) status);
	}
}
/* --- */

int
main()
{
	char cmd[MAX_CMD_LEN];
	int status;

	while (true) {
		// read in command interactively
		fprintf(stderr, "command: ");
		if (NULL == fgets(cmd, MAX_CMD_LEN, stdin)) {
			break;
		}

		// execute command
		status = system(cmd);
		fprintf(stderr, "system() returned: status=0x%04x (%d,%d)\n",
			(unsigned int) status, status >> 8, status & 0xff);

		// evaluate execution
		if (-1 == status) {
			fprintf(stderr, "system() failed\n");
			exit(EXIT_FAILURE);
		} else {
			if (WIFEXITED(status) && WEXITSTATUS(status == 127)) {
				fprintf(stderr, "(probably) could not invoke shell\n");
			} else {
				print_wait_status(NULL, status);
			}
		}
	}

	printf("READY.\n");
	exit(EXIT_SUCCESS);
}
