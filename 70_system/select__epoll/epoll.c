/*
  epoll() - I/O multiplexing the linux way


  usage:

  -- TERMINAL 1 --                      -- TERMINAL 2 --

  $ mkfifo p q
  $ ./epoll.elf ./p ./q

                                        $ cat > ./p
                                                            // CTRL+z
			                ^Z
					[1]+  Stopped

    main():69 - opened "./p" on fd 4

                                        $ cat > ./q

    main():69 - opened "./q" on fd 5
    main():91 - about to epoll_wait()


                                        qqq                 // enter 'qqq'
                                                            // CTRL+d

    main():118 - ready
    main():128 -    fd=5; events: EPOLLIN
    main():148 -    read 3 bytes: qqq
    main():91 - about to epoll_wait()
    main():118 - ready
    main():128 -    fd=5; events: EPOLLHUP
    main():167 -    closing fd 5
    main():91 - about to epoll_wait()

                                        $ fg %1
			                cat > ./p
			                ppp

    main():118 - ready
    main():128 -    fd=4; events: EPOLLIN
    main():148 -    read 4 bytes: ppp
    main():91 - about to epoll_wait()

                                                            // CTRL+d

    main():118 - ready
    main():128 -    fd=4; events: EPOLLHUP
    main():167 -    closing fd 4
    main():91 - about to epoll_wait()


  IO Models

  - IO multiplexing: select()
    select() is portable among unices
    select() is old and classic
    select() is slow and limited

    epoll() is not: neither portable among unices, nor slow and
    limited as select()


  reference: The Linux Programming Interface, Michael Kerrisk, 2010, p. 1362
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_BUF 1000
#define MAX_EVENTS 5

int
main(int argc, char *argv[])
{
	int epfd, ready, fd, ret, idx, nopenfds;
	struct epoll_event ev;
	struct epoll_event evlist[MAX_EVENTS];
	char buf[MAX_BUF];

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s file...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  1.) Create an epoll instance
	 */
	epfd = epoll_create(argc - 1);
	if (-1 == epfd) {
		fprintf(stderr, "epoll_create() failed\n");
		exit(EXIT_FAILURE);
	}

	/*
	  2.) Open each of the files named on the command line for
	      input and...
	*/
	for (idx = 1; idx < argc; idx++) {
		fd = open(argv[idx], O_RDONLY);
		if (-1 == fd) {
			perror("open()");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "%s():%d - opened \"%s\" on fd %d\n",
			__func__, __LINE__, argv[idx], fd);

		ev.events = EPOLLIN; // only interested in input events
		ev.data.fd = fd;
		/*
		  3.) ...add the resulting file descriptor to the
		      interest list of the epoll instance, specifying
		      the set of events to be monitored as EPOLLIN
		*/
		if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev)) {
			fprintf(stderr, "epoll_ctl() failed\n");
			exit(EXIT_FAILURE);
		}
	}
	nopenfds = argc - 1;

	/*
	  4.) Execute a loop that calls epoll_wait()...
	*/
	while (0 < nopenfds) {
		// fetch up to MAX_EVENTS  items from the ready list
		fprintf(stderr, "%s():%d - about to epoll_wait()\n",
			__func__, __LINE__);

		/*
		  5.) ...to monitor the interest list of the epoll
		      instance. Handle the returned events from each
		      call
		 */
		ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
		if (-1 == ready) {
			/*
			  6.) After the epoll_wait() call, the program
	                      checks for an EINTR return, which may
	                      occur if the program was stopped by a
	                      signal in the middle of the epoll_wait()
	                      call and then resumed by SIGCONT. If
	                      this occurs, the program restarts the
	                      epoll_wait() call
			*/
			if (errno == EINTR) {
				continue; // restart if interrupted by a signal
			} else {
				perror("epoll_wait()");
				exit(EXIT_FAILURE);
			}
		}

		fprintf(stderr, "%s():%d - ready\n",
			__func__, __LINE__);

		/*
		  7.) If the epoll_wait() call was successful, the
		      program uses a further loop to check each of the
		      ready items in evlist.
		*/
		for (idx = 0; idx < ready; idx++) {
			// deal with returned list of events
			fprintf(stderr, "%s():%d - \tfd=%d; events: %s%s%s\n",
				__func__, __LINE__, evlist[idx].data.fd,
				(evlist[idx].events & EPOLLIN) ? "EPOLLIN " : "",
				(evlist[idx].events & EPOLLHUP) ? "EPOLLHUP " : "",
				(evlist[idx].events & EPOLLERR) ? "EPOLLERR " : "");

			if (evlist[idx].events & EPOLLIN) {
				/*
				  8.) For each item in evlist, the
				      program checks the events field
				      for the presence of not just
				      EPOLLIN,...
				*/

				ret = read(evlist[idx].data.fd, buf, MAX_BUF);
				if (-1 == ret) {
					perror("read()");
					exit(EXIT_FAILURE);
				}

				fprintf(stderr, "%s():%d - \tread %d bytes: %.*s\n",
					__func__, __LINE__, ret, ret, buf);

			} else if (evlist[idx].events & (EPOLLHUP | EPOLLERR)) {
				/*
				  9.) ...but also EPOLLHUP and
				      EPOLLERR. These latter events
				      can occur if the other end of a
				      FIFO was closed or a terminal
				      hangup occurred. If EPOLLIN was
				      returned, then the program reads
				      some input from the
				      corresponding file descriptor
				      and displays it on standard
				      output. Otherwise, if either
				      EPOLLHUP or EPOLLERR occurred,
				      the program closes the
				      corresponding file descriptor...
				*/
				fprintf(stderr, "%s():%d - \tclosing fd %d\n",
					__func__, __LINE__, evlist[idx].data.fd);

				/*
				  10.) ...and decrements the counter
				       of open files (nopenfds). The
				       loop terminates when all open
				       file descriptors have been
				       closed i.e. when nopenfds
				       equals 0
				*/
				if (-1 == close(evlist[idx].data.fd)) {
					perror("close()");
					exit(EXIT_FAILURE);
				}
				nopenfds++;
			}
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
