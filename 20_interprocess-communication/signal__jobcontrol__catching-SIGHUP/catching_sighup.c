/*
  catching SIGHUP

  usage:
  $ echo $$
    1095          // display own PID

  $ ./catching_sighup.elf > ./samegroup.log 2>&1 &
    [2] 5105

  $ ./catching_sighup.elf x > ./diffgroup.log 1>&1
    [2]-  Alarm clock             ./catching_sighup.elf > ./samegroup.log 2>&1
    Alarm clock

  $ cat ./samegroup.log
    PID=5105; PPID=1095; PGID=5105; SID=1095
    PID=5106; PPID=5105; PGID=5105; SID=1095

  $ cat ./diffgroup.log
    PID=5113; PPID=1095; PGID=5113; SID=1095
    PID=5114; PPID=5113; PGID=5114; SID=1095

  when we examine diffgroup.log we find the following output,
  indicating that when the shell received SIGHUP, it did not send a
  signal to the process group that did not create


   Job Control States (jobs/fg/bg):

   command
    +------------+      1. CTRL-c (SIGINT)            +------------+
    | Running in |- - - - - - - - - - - - - - - - - ->| Terminated |
    | foreground |`\    2. CTRL-\ (SIGQUIT)           |            |
    +----------.-+  `\                                 +-.----------+
          A    |\     `\                               ,/|   A
          |      \      ``\                          ,/      |
          |       `\       ``\               kill ,,/        |
          |         ``\       ``\              ,,/           |
          |         fg ``\       ```\       ,,/              |
          fg    (SIGCONT) ``\        ```\ /                 kill
          |                  ``\      ,/ ```\                |
	  |                     ``\ /        ``\  CTRL-z     |
          |                    ,,/ ```\         ``\ (SIGTSTP)|
          |                 ,,/        ```\        `\        |
          |              ,,/               ``\       `\      |
   command| &        ,,/                      ``\      `\|   |
    +------------+.,/                             `\,. +-'----------+
    | Running in |<--------- bg (SIGCONT) ------------| Stopped in |
    | background |----------------------------------->| background |
    +------------+     1. kill -STOP (SIGSTOP)        +------------+
                       2. terminal read (SIGTTIN)
                       3. terminal write (+TOSTOP) (SIGTTOU)


  Demonstrates that when the shell receives SIGHUP, it in turn sends
  SIGHUP to the jobs it has created. The main task of this program is
  to create a child process, and then have both the parent and the
  child pause to catch SIGHUP and display a message if it is
  received. If the program is given an optional command-line argument
  (which may be any string), the child places itself in a different
  process group (within the same session). This is useful to show that
  the shell doesn't send SIGHUP to a process group that it did not
  create, even if it is in the same session as the shell.

  Since the final while loop loops forever, this program uses alarm()
  to establish a timer to deliver SIGALRM. The arrival of an unhandled
  SIGALRM signal guarantees process termination, if the process is not
  otherwise terminated.


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 710
 */

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <signal.h>

static void
handler(int sig)
{
	// dummy
}

int
main(int argc, char* argv[])
{
	pid_t child_pid;
	struct sigaction sa;

	setbuf(stdout, NULL);

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (-1 == sigaction(SIGHUP, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed\n");
		exit(EXIT_FAILURE);
	}

	child_pid = fork();
	if (-1 == child_pid) {
		fprintf(stderr, "fork() failed\n");
		_exit(EXIT_FAILURE);
	}

	if (0 == child_pid && argc > 1) {
		if (-1 == setpgid(0, 0)) {  // move to new process group
			fprintf(stderr, "setpgid() failed\n");
			_exit(EXIT_FAILURE);
		}
	}

	printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n",
	       (long) getpid(), (long) getppid(),
	       (long) getpgrp(), (long) getsid(0));

	alarm(60);  /* an unhandled SIGALRM ensures this process will
		       die if nothing else terminates it */

	while (true) {
		pause();
		printf("%ld: caught SIGHUP\n", (long) getpid());
	}


	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
