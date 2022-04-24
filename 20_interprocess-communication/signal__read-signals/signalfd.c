/*
  usage:
  $ ./signalfd.elf 44 &
    [1] 27122

--> <ENTER>
    ./signalfd.elf: PID=27122

--> now, send signal '44' to the PID together with some 'data' (123)
  $ ./sigqueue.elf 27122 44 123
    ./sigqueue.elf: PID is 27136, UID is 1000
    main() - READY.
    ./signalfd.elf: got signal 44; ssi_pid = 27136; ssi_int = 123

  $ kill %1
    [1]+  Terminated              ./signalfd.elf 44

--> <ENTER>
    $


  demonstrates how to read signals via struct signalfd_siginfo


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 473
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _GNU_SOURCE /* strsinal() dclaration from <string.h> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

int
main(int argc, char* argv[])
{
	sigset_t mask;
	int sfd, idx;
	struct signalfd_siginfo fdsi;
	ssize_t nbytes;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s sig-num...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "%s: PID=%ld\n", argv[0], (long) getpid());

	sigemptyset(&mask);
	for (idx = 1; idx < argc; idx++) {
		sigaddset(&mask, atoi(argv[idx])); // register passed signum(s)
	}

	if (-1 == sigprocmask(SIG_BLOCK, &mask, NULL)) {
		perror("sigprocmask() failed");
		exit(EXIT_FAILURE);
	}

	// fetching signals via a file descriptor
	sfd = signalfd(-1, &mask, 0);
	if (-1 == sfd) {
		perror("signalfd() failed");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		nbytes = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
		if (nbytes != sizeof(struct signalfd_siginfo)) {
			perror("read() failed");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "%s: got signal %d", argv[0], fdsi.ssi_signo);
		if (fdsi.ssi_code == SI_QUEUE) {
			fprintf(stderr, "; ssi_pid = %d; ", fdsi.ssi_pid);
			fprintf(stderr, "ssi_int = %d", fdsi.ssi_int);
		}
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
