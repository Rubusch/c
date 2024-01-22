/*
  Running just one/single/unique instance of a program


  usage:
  before see which locks are held
  $ cat /proc/locks
    1: FLOCK  ADVISORY  WRITE 1397 00:21:48 0 EOF
    2: FLOCK  ADVISORY  WRITE 342 00:15:641 0 EOF
    3: FLOCK  ADVISORY  WRITE 1021 00:24:48 0 EOF
    4: FLOCK  ADVISORY  WRITE 436 00:15:696 0 EOF

  $ ./flock.elf ./tfile &
    [1] 16708

  $ cat /proc/locks
    1: POSIX  ADVISORY  WRITE 16708 b3:02:1048386 0 EOF
    2: FLOCK  ADVISORY  WRITE 1397 00:21:48 0 EOF
    3: FLOCK  ADVISORY  WRITE 342 00:15:641 0 EOF
    4: FLOCK  ADVISORY  WRITE 1021 00:24:48 0 EOF
    5: FLOCK  ADVISORY  WRITE 436 00:15:696 0 EOF

  now try to start another instance of the program, doing another lock

  $ ./flock.elf ./tfile &
    [2] 16712
    fatal: PID file './tfile' is locked; probably './flock.elf' is already running
      <waiting / blocked>
    READY.

    [1]-  Done                    ./flock.elf ./tfile
    [2]+  Exit 1                  ./flock.elf ./tfile


  A common method of doing this is to have the daemon create a file in
  a standard directory and place a write lock on it. The daemon holds
  the file lock for the duration of its execution and deletes the file
  just before terminating. if another instance of the daemon is
  started, it will fail to obtain a write lock on the
  file. Consequently, it will realize that another instance of the
  daemon must already be running, and terminate.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1143
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 128  /* large enough to hold maximum PID as a string */

/*
  open/create the file named in 'pid_file', lock it, optionally set
  the close-on-exec flag for the file descriptor, write our PID into
  the file, and (in case the caller is interested) return the file
  descriptor referring to the locked file. The caller is responsible
  for deleting 'pid_file' file (just) before process
  termination. 'prog_name' should be the name of th ecalling program
  (i.e. argv[0] or similar), and is used only for diagnostic
  messages. If we can't open 'pid_file', or we encounter some other
  error, then we print an appropriate diagnostic and terminate.
*/

/* --- */
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
create_pid_file(const char *prog_name, const char *pid_file, int flags)
{
	int fd;
	char buf[BUF_SIZE];

	fd = open(pid_file, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	if (-1 == fd) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	if (flags & O_CLOEXEC) { /* legacy linux uses CPF_CLOEXEC instead */

		/*
		  set the close-on-exec file descriptor flag
		*/

		flags = fcntl(fd, F_GETFD);   // fetch flags
		if (-1 == flags) {
			fprintf(stderr, "could not get flags for PID file %s\n", pid_file);
			exit(EXIT_FAILURE);
		}

		flags |= FD_CLOEXEC;   // turn on FD_CLOEXEC

		if (-1 == fcntl(fd, F_SETFD, flags)) {
			fprintf(stderr, "could not set flags for PID file %s\n", pid_file);
			exit(EXIT_FAILURE);
		}
	}

	if (-1 == lock_region(fd, F_WRLCK, SEEK_SET, 0, 0)) {
		if (EAGAIN == errno || EACCES == errno) {
			fprintf(stderr, "fatal: PID file '%s' is locked;\n"
				"probably '%s' is already running\n", pid_file, prog_name);
		} else {
			fprintf(stderr, "unable to lock PID file '%s'\n", pid_file);
		}
		exit(EXIT_FAILURE);
	}

	if (-1 == ftruncate(fd, 0)) {
		fprintf(stderr, "could not truncate PID file '%s'\n", pid_file);
		exit(EXIT_FAILURE);
	}

	snprintf(buf, BUF_SIZE, "%ld\n", (long) getpid());
	if (strlen(buf) != write(fd, buf, strlen(buf))) {
		fprintf(stderr, "fatal: writing to PID file '%s'\n", pid_file);
		exit(EXIT_FAILURE);
	}

	return fd;
}

int
main(int argc, char *argv[])
{
	int ret;
	int flags;

	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <pid file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	flags = FD_CLOEXEC;

	ret = create_pid_file(argv[0], argv[1], flags);
	if (0 > ret) {
		fprintf(stderr, "create_pid_file() failed\n");
		exit(EXIT_FAILURE);
	}

	sleep(100); /* do some waiting for the demo purpose */

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
