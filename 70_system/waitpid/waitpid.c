// wait - part 2
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-September-27



  second example using wait()
  #include <sys/types.h>
  #include <unistd.h>
  pid_t  waitpid(pid_t pid, int* status, int options)

  WIFEXITED() - true if child exited due to exit() or _exit()
  WEXITSTSTUS() - contains the exit code in case of exit() or _exit()
  WIFSIGNALED() - true, if child exited due to a signal
  WIFTERMSIG() - contains the signal number in case of a signal
  WIFSTOPPED() - contains if child stopped (signal)


  #include <sys/wait.h>
  pid_t waitpid(pid_t pid, int* status, int options);

  As wait(), but wait's on a particular child process (pid) which doesn't need
  to be the first one.


  PARAMS

  pid - pid of the child process
  status - ???
  options - 3 possibilities:
      WCONTINUED func shall report the status of any continued child process
                 specified by pid whose status has not been reported since it
                 continued from a job control stop

      WNOHANG    func shall not suspend execution of the calling thread of
                 status is not immediately available for the one of the child
                 process pid

      WUNTRACED  status of any child processes specified by pid that are
                 stopped shall be reported to the requested process


  MACROS

  WIFEXITED()    evaluates to a non-zero value if "status" was returned for a
                 child process that terminated normally

  WEXITSTATUS()  if the value is non-zero, this macro evaluates to the low-order
                 8 bits of "status" that the child process passed to _exit() or
                 exit(), or the value the child pid returned from main()

  WIFSIGNALED()  evaluates to a non-zero value of "status"was returned for a
                 child process that terminated due to the receipt of a signal
                 that was not caught

  WTERMSIG()     if WIFSIGNALED is non-zero, this macro evaluates to the number
                 of the signal that caused the termination of the child pid

  WIFSTOPPED()   evaluates the value of WFISTOPPED is non-zero, this macro
                 evaluates to the number of the signal that caused the child
                 process to stop

  WSTOPSIG()     if the value of WIFSTOPPED is non-zero, this macro evaluates
                 to the number of the signal that caused the child process to
                 stop

  ERRORs

  ECHILD         child does not exist or pid is not a child
  EINVAL         invalid "option"
  EINTR          func was interrupted by a signal
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PARENT_TXT "parent - "
#define CHILD_TXT "\tchild - "
#define IDENTIFIER_SIZE 20

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

	// fork()
	if (0 > (pid = fork())) {
		perror("fork failed");
		_exit(EXIT_FAILURE);

	} else if (pid == 0) {
		// child code
		strncpy(identifier, CHILD_TXT, strlen(CHILD_TXT) + 1);

		printf("%schild pid: %i, parent: %i\r\n", identifier, getpid(),
		       pid_parent);
		printf("%ssleeps\r\n", identifier);
		sleep(5);
		printf("%sawakes\r\n", identifier);
		printf("%sdone\r\n", identifier);
		_exit(EXIT_SUCCESS);

	} else {
		// parent code
		strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT) + 1);
		printf("%swaiting on pid %i\r\n", identifier, pid);

		/*
		  1. set wait condition
		*/
		pid_t pid_wait = -1;

		/*
		  2. child exit status
		*/
		int childExitStatus = 0;

		/*
		  3. wait on child, pid_wait = -1, wait on all childs
		*/
		pid_wait = pid;
		while (0 == waitpid(pid_wait, &childExitStatus, 0))
			;
		printf("%swaiting done\r\n", identifier);

		/*
		  in case: 4. evaluation of term conditions of the child
		*/
		if (WIFEXITED(childExitStatus)) {
			printf("%schild exited with exit(): %i\r\n", identifier,
			       WEXITSTATUS(childExitStatus));
		}

		if (WIFSIGNALED(childExitStatus)) {
			printf("%schild exited with a SIGNAL: %i\r\n",
			       identifier, WIFSIGNALED(childExitStatus));
		}

		if (WTERMSIG(childExitStatus)) {
			printf("%schild exited with SIGTERM: %i\r\n",
			       identifier, WTERMSIG(childExitStatus));
		}

		if (WIFSTOPPED(childExitStatus)) {
			printf("%schild exited with SIGSTOP: %i\r\n",
			       identifier, WIFSTOPPED(childExitStatus));
		}

		printf("%sdone\r\n", identifier);
		exit(EXIT_SUCCESS);
	}
}
