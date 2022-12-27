/*
  The self-pipe trick

  usage:
    $ ./self-pipe-trick.elf
      x
      x
      x
      x
      ^Ca signal was caught
      READY.


  The ppoll() and epoll_pwait() system calls

  Linux 2.6.16 also added a new, nonstandard system call, ppoll(),
  whose relationship to poll() is analogous to the relationship of
  pselect() to sleect(). Similarly, starting with kernel 2.6.19, Linux
  also includes epoll_pwait(), providing an analogous extension to
  epoll_wait(). See the ppoll() and epoll_pwait() manual pages for
  details.

  NB: ppoll(), epoll_pwait() - similar to epoll but with signal
  handling


  Since pselet() is not widely implemented, portable applications must
  employ other strategies to avoid race conditions when simultaneously
  waiting for signals and calling select() on a set of file
  descriptors.

  One common solution is the technique commonly known as the
  pipe-trick:

  1. Create a pipe, and mark its read and write ends as nonblocking

  2. As well as monitoring all of the other file descriptors that are
     of interest, include the read end of the pipe in the readfds set
     given to select()

  3. Install a handler for the signal that is of interest. When this
     signal handler is called, it writes a byte of data to the pipe. Note
     the following points about the signal handler:

     - The write end of the pipe was marked as nonblocking in the
       first step to prevent the possibility that signals arraive so
       rapidly that repeated invocations of the signal handler fill
       the pipe, with the result that the signal handler's write() and
       thus the process itself is blocked. It doesn't matter if a
       write to a full pipe fails, since the previous writes will
       already have indicated the delivery of the signal.

     - The signal handler is installed after creating the pipe, in
       order to prevent the race condition that would occur if a
       singla was delivered before the pipe was created.

     - It is safe to use write() inside the signal handler, because it
       is one of the async-signal-safe functions listed [TLPI,
       p. 426].

  4. Place the select() call in a loop, so that it is restarted if
     interrupted by a signal handler. (Restarting in this fashion is
     not strictly necessary; it merely means that we can check for the
     arrival of a signal by inspecting readfds, rather than checking
     for an EINTR error return).

  5. On successful completion of the select() call, we can determine
     whether a signal arrived by checking if the file descriptor for
     the read end of the pipe is set in readfds.

  6. Whenever a signal has arrived, read all bytes that are in the
     pipe. Since multiple signals may arrive, employ a loop that reads
     bytes until the (nonblocking) read() fails with the error
     EAGAIN. After draining the pipe, perform whatever actions must be
     taken in response to delivery of the signal.


  references:
  The Linux Programming Interface, Michael Kerrisk, 2010, p. 1370
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h> // MIN(), MAX()
// alternatively, define
//#define MIN(m,n) ((m) < (n) ? (m) : (n))
//#define MAX(m,n) ((m) > (n) ? (m) : (n))
#include <signal.h>
#include <stdbool.h>
#include <errno.h>


static int pfd[2];    // file descriptors for pipe


static void
handler(int sig)
{
	int saved_errno;

	saved_errno = errno;   // in case we changed errno
	if (-1 == write(pfd[1], "x", 1)
	    && errno != EAGAIN) {
		perror("write()");
		_exit(EXIT_FAILURE);
	}
	errno = saved_errno;
}

int
main(int argc, char *argv[])
{
	fd_set readfds;
	int ready, nfds, flags;
//	struct timeval timeout; // use in case
	struct timeval *pto;
	struct sigaction sa;
	char ch;

	/*
	  ... initialize 'timeout', 'readfds', and 'nfds' for select()
	*/
	ready = 0;
	nfds = 0;
	flags = 0;
	pto = NULL;
	ch = 0;

	if (-1 == pipe(pfd)) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	FD_SET(pfd[0], &readfds);      // add read end of pipe to 'readfds'
	nfds = MAX(nfds, pfd[0] + 1);  // and adjust 'nfds' if required

	flags = fcntl(pfd[0], F_GETFL);
	if (-1 == flags) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	flags |= O_NONBLOCK;        // make read end nonblocking
	if (-1 == fcntl(pfd[0], F_SETFL, flags)) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}

	flags = fcntl(pfd[1], F_GETFL);
	if (-1 == flags) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	flags |= O_NONBLOCK;        // make write end nonblocking
	if (-1 == fcntl(pfd[1], F_SETFL, flags)) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;   // restart interrupted read()
	sa.sa_handler = handler;
	if (-1 == sigaction(SIGINT, &sa, NULL)) {
		perror("sigaction()");
		exit(EXIT_FAILURE);
	}

	while (-1 == (ready = select(nfds, &readfds, NULL, NULL, pto))
		&& errno == EINTR) {
		continue;   // restart if interrupted by signal
	}

	if (-1 == ready) {  // unexpected error
		perror("select()");
		exit(EXIT_FAILURE);
	}

	if (FD_ISSET(pfd[0], &readfds)) {       // handler was called
		fprintf(stderr, "a signal was caught\n");

		while (true) {
			if (-1 == read(pfd[0], &ch, 1)) {
				if (EAGAIN == errno) {
					break;  // no more bytes
				} else {
					fprintf(stderr, "read\n");
					exit(EXIT_FAILURE);
				}
			}

			/*
			  perform any actions that should be taken in
			  response to signal
			*/
		}
	}

	/*
	  examine file descriptor sets returned by select() to see
	  which other file descriptors are ready
	*/

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
