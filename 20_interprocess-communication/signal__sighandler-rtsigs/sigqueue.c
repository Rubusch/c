/*
  usage of ./sighandler.elf and sigqueue.elf

  $ ./sighandler.elf 60 &
  [8] 5732

  $ ./sighandler.elf: PID is 5732
      ./sighandler.elf: signals blocked - sleeping 60 seconds

  $ ./sigqueue.elf 5732 40 200
      ./sigqueue.elf: PID is 5736, UID is 1000
      main() - READY.

  $ ./sigqueue.elf 5732 43 300
      ./sigqueue.elf: PID is 5744, UID is 1000
      main() - READY.

  $ ./sigqueue.elf 5732 50 100
      ./sigqueue.elf: PID is 5748, UID is 1000
      main() - READY.

  --> after a while, sighandler will report the caught signals...

  $ ./sighandler.elf: sleep complete
      caught signal 40
        si_signo = 40, si_code = -1 (SI_QUEUE), si_value = 200
        si_pid = 5736, si_uid = 1000
      caught signal 43
        si_signo = 43, si_code = -1 (SI_QUEUE), si_value = 300
        si_pid = 5744, si_uid = 1000
      caught signal 50
        si_signo = 50, si_code = -1 (SI_QUEUE), si_value = 100
        si_pid = 5748, si_uid = 1000



  demostrates a signal handler dealing with asynchronous signals
  i.e. "realtime" signals, this demonstrates a regular signalhandler

  sighandler.elf: start detached and use the PID obtained
  sigqueue.elf: sends signals to the particular PID

  use SIGINT or SIGTERM to terminate program


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 462
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _GNU_SOURCE /* strsinal() dclaration from <string.h> */
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

#include "tlpi/get_num.h"

int
main(int argc, char* argv[])
{
	int sig, nsigs, idx, sig_data;
	union sigval sv;

	if (argc < 4) {
		fprintf(stderr, "usage:\n$ %s <pid> <signum> <data> [num-sigs]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  display our PID and UID, so that they can be compared with
	  the corresponding fields of the siginfo_t argument supplied
	  to the handler in the receiving process
	*/

	fprintf(stderr, "%s: PID is %ld, UID is %ld\n",
		argv[0], (long) getpid(), (long) getuid());

	sig = get_int(argv[2], 0, "sig_num");
	sig_data = get_int(argv[3], GN_ANY_BASE, "data");
	nsigs = (argc > 4) ? get_int(argv[4], GN_GT_0, "num_sigs") : 1;

	for (idx = 0; idx < nsigs; idx++) {
		sv.sival_int = sig_data + idx;
		if (-1 == sigqueue(get_long(argv[1], 0, "pid"), sig, sv)) {
			perror("sigqueue() failed");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
