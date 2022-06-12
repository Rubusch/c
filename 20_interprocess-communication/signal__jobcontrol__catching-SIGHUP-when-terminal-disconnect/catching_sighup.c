/*
  catching SIGHUP

  usage:
  $ exec ./catching_sighup.elf d s s > sig.log 2>&1
    <exits the terminal>

  $ cat ./sig.log
    main():67 - PID of parent process is:   27812    // parent PG group
    main():69 - Foreground process group ID is: 27812
    main():114 - PID=14193 PGID=14193                // first child is in a different process group
    main():114 - PID=27812 PGID=27812                // remaining children are in same PG as parrent
    main():114 - PID=14194 PGID=27812
    main():114 - PID=14195 PGID=27812
    <PID 14194 caught signal 1 (HANGUP)>
    <PID 14195 caught signal 1 (HANGUP)>


  Suppose that we run the program in a terminal window via exec. The
  Exec command is a shell built-in command that causes the shell to do
  an exec(), replacing itself with the named program. Since the shell
  was the controlling proreplacing itself with the named
  program. Since the shell was the controlling process for the
  terminal, our program is now the controling process and will receive
  SIGHUP when the terminal window is closed. After closing the
  terminal window, we find the following lines in the file sig.log.

  Closeing the terminal window caused SIGHUP to be sent to the
  controlling process (the parent), which terminated as a result. We
  see that the two children that were in the same process group as the
  parent (i.e., the foreground process group for the terminal) also
  both received SIGHUP. However, the child that was in a separate
  (background) process group did not receive the signal.


  Generally...
     CTRL-z SIGSTP   -> job to background (bg)
            SIGCONT  -> job to foreground (fg)

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


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 712
 */

#define _GNU_SOURCE  /* strsignal() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <signal.h>

static void
handler(int sig)
{
	/*
	  1.) When a signal is delivered the handler prints the
	  process ID of the process and signal number.
	*/
	printf("%s():%d - PID %ld: caught signal %2d (%s)\n",
	       __func__, __LINE__, (long) getpid(), sig, strsignal(sig));
	// NB: using stdio, printf() & CO in signal handler is not "asynch signal safe"!!!
}

int
main(int argc, char* argv[])
{
	pid_t parent_pid, child_pid;
	int idx;
	struct sigaction sa;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s {d|s}... [ > sig.log 2>&1 ]\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	setbuf(stdout, NULL);

	parent_pid = getpid();
	printf("%s():%d - PID of parent process is:\t%ld\n",
	       __func__, __LINE__, (long) parent_pid);
	printf("%s():%d - Foreground process group ID is: %ld\n",
	       __func__, __LINE__, (long) tcgetpgrp(STDIN_FILENO));

	/*
	  2.) create child
	*/
	for (idx = 1; idx < argc; idx++) {   // create child processes
		child_pid = fork();
		if (-1 == child_pid) {
			perror("fork()");
			exit(EXIT_FAILURE);
		}

		if (0 == child_pid) {
			/*
			  3.) If the corresponding command-line
			  argument is the letter d, then the child
			  process places itself in its own (different)
			  process group.
			*/
			if ('d' == argv[idx][0]) {
				if (-1 == setpgid(0, 0)) {
					perror("setpgid()");
					exit(EXIT_FAILURE);
				}
			}

			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = handler;
			/*
			  4.) Each child then establishes a handler for SIGHUP
			*/
			if (-1 == sigaction(SIGHUP, &sa, NULL)) {
				fprintf(stderr, "sigaction() failed\n");
				exit(EXIT_FAILURE);
			}
			break;   // child exits loop
		}
	}

	/* all processes fall through to here */

	alarm(60);  // ensure each process eventually terminates

	printf("%s():%d - PID=%ld PGID=%ld\n",
	       __func__, __LINE__, (long) getpid(), (long) getpgrp());

	while (true) {
		pause();
		printf("%s():%d - %ld: caught SIGHUP\n",
		       __func__, __LINE__, (long) getpid());
	}


	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
