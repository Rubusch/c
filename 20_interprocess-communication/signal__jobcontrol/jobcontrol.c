/*
  jobcontrol

  usage:
  $ ./jobcontrol.elf
    terminal FG process group: 3251
    command  PID  PPID  PGRP   SID
       1    3251   1136   3251   1136

  <CTRL+z, send SIGTSTP>
    ^Z
    terminal FG process group: 3251
    process 3251 (1) received signal 20 (Stopped)

    [1]+  Stopped                 ./jobcontrol.elf


  $ jobs
    [1]+  Stopped                 ./jobcontrol.elf

  $ fg
    ./jobcontrol.elf
    terminal FG process group: 3251
    process 3251 (1) received signal 18 (Continued)

  <CTRL+c, send SIGINT - is caught>
    ^C
    terminal FG process group: 3251
    process 3251 (1) received signal 2 (Interrupt)

  <CTRL+\, send SIGSTOP - stops the process>
    Quit


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
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 717
 */

#define _GNU_SOURCE /* strsignal() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

static int cmdidx;  // position in pipeline

static void
handler(int sig)
{
	// NB: using stdio is not async signal safe!!!

	/*
	  1.) Display the foreground process group for the terminal
	*/
	if (getpid() == getpgrp()) {
		fprintf(stderr, "terminal FG process group: %ld\n",
			(long) tcgetpgrp(STDERR_FILENO));
	}

	/*
	  2.) Display the ID of the process, the process's position in
	  the pipeline, and the signal received
	*/
	fprintf(stderr, "process %ld (%d) received signal %d (%s)\n",
		(long) getpid(), cmdidx, sig, strsignal(sig));

	/*
	  3.) The handler must do some extra work if it catches SIGTSTP, since, when caught, this signal doesn't stop a process

	  If we catch SIGSTP, it won't actually stop us. Therefore we
	  raise SIGSTOP so we actually get stopped
	*/

	if (SIGTSTP == sig) {
		raise(SIGSTOP);
	}
}

int
main(int argc, char* argv[])
{
	struct sigaction sa;

	setbuf(stdout, NULL);

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
	if (-1 == sigaction(SIGINT, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed\n");
		exit(EXIT_FAILURE);
	}
	if (-1 == sigaction(SIGTSTP, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed\n");
		exit(EXIT_FAILURE);
	}
	if (-1 == sigaction(SIGCONT, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed\n");
		exit(EXIT_FAILURE);
	}

	/*
	  if stdin is a terminal, this is the first process in pipeline:
	  print a heading and initialize message to be sent down pipe
	*/

	if (isatty(STDIN_FILENO)) { // uses isatty() to check if it is a terminal
		fprintf(stderr, "terminal FG process group: %ld\n",
			(long) tcgetpgrp(STDIN_FILENO));
		fprintf(stderr, "command  PID  PPID  PGRP   SID\n");
		cmdidx = 0;
	} else {
		if (0 >= read(STDIN_FILENO, &cmdidx, sizeof(cmdidx))) {
			fprintf(stderr, "FATAL: read got EOF\n");
			_exit(EXIT_FAILURE);
		}
	}

	cmdidx++;
	fprintf(stderr, "%4d   %5ld  %5ld  %5ld  %5ld\n",
		cmdidx, (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));

	/* if not the last process, pass a message to the next process */

	if (!isatty(STDOUT_FILENO)) {   // if not a tty, then should be pipe
		if (-1 == write(STDOUT_FILENO, &cmdidx, sizeof(cmdidx))) {
			perror("write()");
			_exit(EXIT_FAILURE);
		}
	}

	while (true) {
		pause();
	}

	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
