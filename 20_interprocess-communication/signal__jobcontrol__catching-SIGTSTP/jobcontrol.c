/*
  jobcontrol - catching SIGTSTP

  usage:
  $ ./jobcontrol.elf
      // running

      <CTRL-z, sending SIGTSTP>
    ^Z caught SIGTSTP

    [1]+  Stopped                 ./jobcontrol.elf

  $ fg
    ./jobcontrol.elf
    exiting SIGTSTP handler
    main()
      // running, again - SIGTSTP handler is re-established

      <CTRL-c, send SIGINT to stop the program>
    ^C



   Dealing with the gnored signal and setting up a catching of SIGTSTP

   1. The handler resets the disposition of SIGTSTP to its default
      (SIG_DFL)

   2. The handler raises SIGTSTP

   3. Since SIGTSTP was blocked on entry to the handler (unless the
      SA_NODEFER flag was specified), the handler unblocks this
      signal. At this point, the pending SIGTSTP raised in the
      previous step performs its default action: the process is
      immediately suspended

   4. At some later time, the process will be resumed upon receipt of
      SIGCONT. At this point, execution of the handler continues

   5. Before returning, the handler reblocks the SIGTSTP signal and
      reestablishes itself to handle the next occurrence of the
      SIGTSTP signal


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
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 724
 */

#define _GNU_SOURCE /* strsignal() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <errno.h>
#include <signal.h>

/*
  handler for SIGTSTP
 */
static void
handler(int sig)
{
	sigset_t mask_tstp, mask_prev;
	int errno_saved;
	struct sigaction sa;

	/*
	  ATTENTION: using stdio is not asynch-signal-save, thus discarded!!!
	*/

	errno_saved = errno; // in case we change 'errno' here

	fprintf(stderr, "caught SIGTSTP\n");
	if (SIG_ERR == signal(SIGTSTP, SIG_DFL)) { // set handling to default
		fprintf(stderr, "signal() failed, setting handling to default\n");
		_exit(EXIT_FAILURE);
	}
	raise(SIGTSTP); // generate a further SIGTSTP

	/*
	  unblock SIGTSTP - the pending SIGTSTP immediately suspends
	  the program
	*/
	sigemptyset(&mask_tstp);
	sigaddset(&mask_tstp, SIGTSTP);
	if (-1 == sigprocmask(SIG_UNBLOCK, &mask_tstp, &mask_prev)) {
		fprintf(stderr, "sigprocmask() failed, setting tstp mask");
		_exit(EXIT_FAILURE);
	}

	/*
	  execution resumes here after SIGCONT
	*/
	if (-1 == sigprocmask(SIG_SETMASK, &mask_prev, NULL)) { // re-block SIGTSTP
		fprintf(stderr, "sigprocmask() failed, (re-)setting previous mask\n");
		_exit(EXIT_FAILURE);
	}

	// re-establishing the handler
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
	if(-1 == sigaction(SIGTSTP, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed, (re-)establishing signal handler");
		_exit(EXIT_FAILURE);
	}

	fprintf(stderr, "exiting SIGTSTP handler\n");
	errno = errno_saved; // re-establishing errno
}

int
main(int argc, char* argv[])
{
	struct sigaction sa;

	setbuf(stdout, NULL);

	/*
	  only establish handler for SIGTSTP if it is not being ignored
	 */
	if (-1 == sigaction(SIGTSTP, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed\n");
		exit(EXIT_FAILURE);
	}

	if (sa.sa_handler != SIG_IGN) {
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		sa.sa_handler = handler;

		if (-1 == sigaction(SIGTSTP, &sa, NULL)) {
			fprintf(stderr, "sigaction() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	while (true) { // wait for signals
		pause();
		fprintf(stderr, "%s()\n", __func__);
	}

	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
