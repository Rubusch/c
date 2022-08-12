/*
  file lock demo

  usage:

  We begin by creating a file /tmp/tfile, and then start an instance
  of our program that sits in the background and holds a shared lock
  for 60 seconds.

  $ touch /tmp/tfile

  $ ./flock.elf /tmp/tfile s 60 &
    [1] 6105
    PID 6105: requesting LOCK_SH at 22:47:06
    PID 6105: granted       LOCK_SH at 22:47:06

  Next, we start another instance of the program that successfully
  requests a shared lock and then releases it.

  $ ./flock.elf /tmp/tfile s 2
    PID 6112: requesting LOCK_SH at 22:47:19
    PID 6112: granted       LOCK_SH at 22:47:19
    PID 6112: releasing     LOCK_SH at 22:47:21
    READY.

  However, when we start another instance of the program that makes a
  nonblocking requests for an exclusive lock, the request immediately
  fails

  $ ./flock.elf /tmp/tfile xn
    PID 6116: requesting LOCK_EX at 22:47:32
    PID 6116: already locked - bye!
    PID 6116: granted       LOCK_EX at 22:47:32
    PID 6116: releasing     LOCK_EX at 22:47:42
    READY.

  When we start another instance of the program that makes a blocking
  request for an exclusive lock, the program blocks. When the
  background process that was holding a shared lock for 60 seconds
  releases its lock, the blocked request is granted.

  $ ./flock.elf /tmp/tfile x
    PID 6126: requesting LOCK_EX at 22:47:47
    PID 6105: releasing     LOCK_SH at 22:48:06
    READY.
    PID 6126: granted       LOCK_EX at 22:48:06
    PID 6126: releasing     LOCK_EX at 22:48:16
    READY.
    [1]+  Done                    ./flock.elf /tmp/tfile s 60


  The listing demonstrates the use of flock(). This program locks a
  file, sleeps for a specified number of seconds, and then unlocks the
  file. The program takes up to three command-line arguments. The
  first of these is the file to lock. The second specifies the lock
  type (shared or exclusive) and whether or not to include the LOCK_NB
  (nonblocking) flag. The third argument specifies the number of
  seconds to sleep between acquiring and releasing the lock; this
  argument is optional and defaults to 10 seconds.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1120
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "tlpi/get_num.h"


/*
  current_time()

  Return a string containing the current time formatted according to
  the specification in 'format' (see strftime(3) for specifiers). If
  'format' is NULL, we use "%c" as a specifier (which gives the date
  and time as for ctime(3), but without the trailing newline). Returns
  NULL on error.

  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 194
*/
#ifndef BUFSIZ
# define BUFSIZ 1024
#endif

char*
curr_time(const char *format)
{
        static char buf[BUFSIZ]; // non-reentrant!!!
        time_t t;
        size_t ret;
        struct tm *tm;

        t = time(NULL);
        tm = localtime(&t);
        if (NULL == tm) {
                return NULL;
        }

        ret = strftime(buf, BUFSIZ, (format != NULL) ? format : "%c", tm);

        return (0 == ret) ? NULL : buf;
}

/* --- */


int
main(int argc, char *argv[])
{
	int fd, lock;
	const char *lname;

	if (3 > argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <filename> <shared|exclusive> [sleep-time]\n"
			"\tlock is 's' (shared) or 'x' (exclusive)\n"
			"\t\toptionally followed by 'n' (nonblocking)\n"
			"\t'sleep-time' specifies time to hold lock\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
	if (argv[2][1] == 'n') {
		lock |= LOCK_NB;
	}

	fd = open(argv[1], O_RDONLY);  // open file to be locked
	if (-1 == fd) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";

	fprintf(stderr, "PID %ld: requesting %s at %s\n",
		(long) getpid(), lname, curr_time("%T"));

	if (-1 == flock(fd, lock)) {
		if (errno == EWOULDBLOCK) {
			fprintf(stderr, "PID %ld: already locked - bye!\n",
				(long) getpid());
		} else {
			fprintf(stderr, "flock (PID=%ld)\n",
				(long) getpid());
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "PID %ld: granted\t%s at %s\n",
		(long) getpid(), lname, curr_time("%T"));

	sleep((3 < argc) ? get_int(argv[3], GN_NONNEG, "sleep-time") : 10);
	fprintf(stderr, "PID %ld: releasing\t%s at %s\n",
		(long) getpid(), lname, curr_time("%T"));
	if (-1 == flock(fd, LOCK_UN)) {
		perror("flock()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
