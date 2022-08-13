/*
  file lock demo - region locking

  usage:
  $ ./flock.elf ./tfile
    lock_region(<lock>)
    region_is_locked() == 0
    lock_region(<unlock>)
    READY.


  A set of locking functions using F_SETLK to place a lock on the open
  file referred to by the file descriptor fd.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1134
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>

/**
  private: lock a file region (private; public interfaces below)
*/
static int
_lock_region(int fd, int cmd, int type, int whence, int start, off_t len)
{
	struct flock fl;

	fl.l_type = type;
	fl.l_whence = whence;
	fl.l_start = start;
	fl.l_len = len;

	// fcntl(descriptor, set/get/..., flock)
	return fcntl(fd, cmd, &fl);
}

/**
  lock a file region using nonblocking F_SETLK
*/
int
lock_region(int fd, int type, int whence, int start, int len)
{
	return _lock_region(fd, F_SETLK, type, whence, start, len);
}

/**
  lock a file region using blocking F_SETLKW
*/
int
lock_region_wait(int fd, int type, int whence, int start, int len)
{
	return _lock_region(fd, F_SETLKW, type, whence, start, len);
}

/*
  test if a file region is lockable, return 0 if lockable, or PID of
  process holding incompatible lock, or -1 on error.
*/

pid_t
region_is_locked(int fd, int type, int whence, int start, int len)
{
	struct flock fl;
	int ret;

	fl.l_type = type;
	fl.l_whence = whence;
	fl.l_start = start;
	fl.l_len = len;
	ret = fcntl(fd, F_GETLK, &fl);
	if (-1 < ret) {
		return (fl.l_type == F_UNLCK) ? 0 : fl.l_pid;
	}
	return ret;
}
/* --- */


int
main(int argc, char *argv[])
{
	int ret;
	int fd;
	int type;
	int whence;
	int start;
	int len;

	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <lock file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	if (-1 == fd) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	type = F_RDLCK;

	whence = SEEK_SET;

	start = 0; // assume a 100 byte lock file "tfile" (created by Makefile)

	len = 40;  // lock first 4 bytes of the lock file

	fprintf(stderr, "lock_region(<lock>)\n");
	lock_region(fd, type, whence, start, len);

//	fprintf(stderr, "lock_region_wait(<lock>)\n");
//	lock_region_wait(fd, type, whence, start, len);

	ret = region_is_locked(fd, type, whence, start, len);
	fprintf(stderr, "region_is_locked() == %ld\n", (long) ret);

	type = F_UNLCK;
	fprintf(stderr, "lock_region(<unlock>)\n");
	lock_region(fd, type, whence, start, len);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
