/*
  Usage:
  $ make clean && make

  $ ./sigaction.exe &
  $ Now test the signal hander and send signal SIGTERM to the pid of this demo.
      waiting on signal..

  $ kill -15 $(pidof sigaction.exe)
      sig_handler() - signal: 15, pid: 2834, uid: 1000
      [1]+  Done                    ./sigaction.exe


---

  The siginfo_t data type is a structure with the following fields:

           siginfo_t {
               int      si_signo;     // Signal number
               int      si_errno;     // An errno value
               int      si_code;      // Signal code
               int      si_trapno;    // Trap number that caused
                                         hardware-generated signal
                                         (unused on most architectures)
               pid_t    si_pid;       // Sending process ID
               uid_t    si_uid;       // Real user ID of sending process
               int      si_status;    // Exit value or signal
               clock_t  si_utime;     // User time consumed
               clock_t  si_stime;     // System time consumed
               union sigval si_value; // Signal value
               int      si_int;       // POSIX.1b signal
               void    *si_ptr;       // POSIX.1b signal
               int      si_overrun;   // Timer overrun count;
                                         POSIX.1b timers
               int      si_timerid;   // Timer ID; POSIX.1b timers
               void    *si_addr;      // Memory location which caused fault
               long     si_band;      // Band event (was int in
                                         glibc 2.3.2 and earlier)
               int      si_fd;        // File descriptor
               short    si_addr_lsb;  // Least significant bit of address
                                         (since Linux 2.6.32)
               void    *si_lower;     // Lower bound when address violation
                                         occurred (since Linux 3.19)
               void    *si_upper;     // Upper bound when address violation
                                         occurred (since Linux 3.19)
               int      si_pkey;      // Protection key on PTE that caused
                                         fault (since Linux 4.6)
               void    *si_call_addr; // Address of system call instruction
                                         (since Linux 3.5)
               int      si_syscall;   // Number of attempted system call
                                         (since Linux 3.5)
               unsigned int si_arch;  // Architecture of attempted system call
                                         (since Linux 3.5)
           }

*/
#define _XOPEN_SOURCE 600 /* struct sigaction with struct siginfo_t */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static void sig_handler(int sig, siginfo_t *siginfo, void *context)
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426
	fprintf(stdout, "%s() - signal: %d, pid: %ld, uid: %ld\n", __func__,
		sig, (long)siginfo->si_pid, (long)siginfo->si_uid);
	exit(EXIT_SUCCESS);
}

/*
*/
int main(int argc, char *argv[])
{
	struct sigaction sa;

	fprintf(stdout,
		"Now test the signal hander and send signal SIGTERM to the pid of this demo.\n");
	memset(&sa, '\0', sizeof(sa));

	// use sa-sigaction when the handler has more than one parameters
	sa.sa_sigaction = &sig_handler;

	// enable SA_SIGINFO when using siginfo_t for information passing
	sa.sa_flags = SA_SIGINFO;

	// setup the siginfo signal handler for signal SIGTERM
	if (0 > sigaction(SIGTERM, &sa, NULL)) {
		perror("sigaction failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "waiting on signal..\n");
	while (1) {
		sleep(10);
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
