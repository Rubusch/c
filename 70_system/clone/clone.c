/*
  usage:
  $ ./clone.elf                         // doesn't use CLONE_FILES
    child has terminated
    write() on file descriptor 3 succeeded  // child's close() did not affect parent
    READY.


  The clone() flags bit-mask values

  CLONE_CHILD_CLEARTID        clear ctid when child calls exec() or _exit()
  CLONE_CHILD_SETTID          write thread ID of child into ctid
  CLONE_FILES                 parent and child share table of open FDs
  CLONE_FS                    parent and child share attributes related to FS
  CLONE_IO                    child shares parent's I/O context
  CLONE_NEWIPC                child gets new system V IPC namespace
  CLONE_NEWNET                child gets new network namespace
  CLONE_NEWNS                 child gets copy of parent's mount namespace
  CLONE_NEWPID                child gets new process-ID namespace
  CLONE_NEWUSER               child gets new user-ID namespace
  CLONE_NEWUTS                child gets new UTS namespace
  CLONE_PARENT                make child's parent same as caller's parent
  CLONE_PARENT_SETTID         write thread ID of child into ptid
  CLONE_PID                   obsolete flag used only by system boot process
  CLONE_PTRACE                if parent is being traced, then trace child also
  CLONE_SETTLS                tls describes thread-local storage for child
  CLONE_SIGHAND               parent and child share signal dispositions
  CLONE_SYSVSEM               parent and child share semaphore undo values
  CLONE_THREAD                place child in same thread group as parent
  CLONE_UNTRACED              can't force CLONE_PTRACE on child
  CLONE_VFORK                 parent is suspended until child calls exec() or _exit()
  CLONE_VM                    parent and child share virtual memory

  Linux implements fork() via clone().

  In Linux, fork() is implemented through the use of copy-on-write
  memory pages. Copy-on-write (or COW) is a technique to delay or
  altogether prevent copying of the data, memory pages, etc.

  The difference between thread, lightweight process, process, etc. is
  mainly a difference of CLONE_ flags applied at the specific clone()
  e.g. in fork(), vfork() or pthread_create().

  reference: Linux Kernel Development, R. Love, 2011, p 30ff


  The demo shows using the clone() system call to create a child
  process.

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


  references: The Linux Programming Interface, Michael Kerrisk, 2010, p.601
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
