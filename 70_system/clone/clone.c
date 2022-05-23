/*
  usage:
  $ ./clone.elf                         // doesn't use CLONE_FILES
    child has terminated
    write() on file descriptor 3 succeeded  // child's close() did not affect parent
    READY.


  Using the clone() system call to create a child process.

  - Open a file descriptor for /dev/null that will be closed by the
    child

  - Set the value for the clone() flags argument to CLONE_FILES if a
    command-line argument was supplied, so that the parent and child
    will share a single file argument was supplied, so that the parent
    and child will share a single file descriptor table. If no
    command-line argument was supplied, flags is set to 0.

  - Allocate a stack for use by the child

  - If CHILD_SIG is nonzero and is not equal to SIGCHLD, ignore it, in
    case it is a signal that wuld terminate the process. We don't
    ignore SIGCHLD, because doing so would prevent waiting on the
    child to collect its status.

  - Call clone() to create the child. The third (bit-mask) argument
    includes the termination signal. The fourth argument (func_arg)
    specifies the file descriptor opened earlier

  - Wait for the child to terminate

  - Check whether the file descriptor (opened earlier) is still open
    by trying to write() to it. The program reports whether the
    write() succeds or fails.


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p.601
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>

#include <unistd.h>

// signal to be generated on termination of cloned child
#ifndef CHILD_SIG
# define CHILD_SIG SIGUSR1
#endif

static int
child_func(void *arg) // startup function for cloned child
{
	if (-1 == close(*((int*) arg))) {
		fprintf(stderr, "close() failed\n");
		_exit(EXIT_FAILURE);
	}

	return 0; // child terminates now
}

int
main(int argc, char *argv[])
{
	const int STACK_SIZE = 65536; // stack size for cloned child
	char *stack;          // start of stack buffer
	char *stack_top;      // end of stack buffer
	int ret, fd, flags;

	fd = open("/dev/null", O_RDWR); // child will close this fd
	if (-1 == fd) {
		fprintf(stderr, "open() failed\n");
		exit(EXIT_FAILURE);
	}

	/*
	  If argc > 1, child shares file descriptor table with parent
	 */
	flags = (argc > 1) ? CLONE_FILES : 0;

	/*
	  Allocated stack for child
	*/
	stack = malloc(STACK_SIZE);
	if (NULL == stack) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}

	stack_top = stack + STACK_SIZE;   // assume stack grows downward

	/*
	  Ignore CHILD_SIG, in case it is a signal whose default is to
	  terminate the process; but don't ignore SIGCHLD (which is
	  ignored by default), since that would prevent creation of a
	  zombie.
	  */
	if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD) {
		if (SIG_ERR == signal(CHILD_SIG, SIG_IGN)) {
			fprintf(stderr, "signal() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	/*
	  Create child; child commences execution in child_func()
	*/
	if (-1 ==  clone(child_func, stack_top, flags | CHILD_SIG, (void*) &fd)) {
		fprintf(stderr, "clone() failed\n");
		exit(EXIT_FAILURE);
	}

	/*
	  Parent falls through to here. Wait for child; __WCLONE is
	  needed for child notifying with signal other than SIGCHLD
	*/
	if (-1 == waitpid(-1, NULL, (CHILD_SIG != SIGCHLD) ? __WCLONE : 0)) {
		fprintf(stderr, "waitpid() failed\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "child has terminated\n");

	/*
	  Did close() of file descriptor in child affect parent?
	*/
	ret = write(fd, "x", 1);
	if (-1 == ret && errno == EBADF) {
		fprintf(stderr, "file descriptor %d has been closed\n", fd);
	} else if (-1 == ret) {
		fprintf(stderr, "write() on file descriptor %d failed "
			"unexpectedly (%s)\n", fd, strerror(errno));
	} else {
		fprintf(stderr, "write() on file descriptor %d succeeded\n", fd);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
