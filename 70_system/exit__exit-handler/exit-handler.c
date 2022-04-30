/*
  usage:
    $ ./exit-handler.elf
      READY.
      on_exit_func() called: status=0, arg=20
      atexit_func2() called
      atexit_func1() called
      on_exit_func() called: status=0, arg=10


  exit()

  - exit handlers (functions registered with atexit() and on_exit()
    are called, in reverse order of their registration

  - the stdio stream buffers are flushed

  - the _exit() system call is invoked, using the value supplied in
    status

  NB:
  prefer exit() in the parent, prefer _exit() in children or signal
  handlers, and prefer pthread_exit() for threads

  demonstrates the use of atexit() and on_exit() to register exit
  handlers. when we run this program, we see the folowing output.


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 536
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void
atexit_func1(void)
{
	fprintf(stderr, "%s() called\n", __func__);
}

static void
atexit_func2(void)
{
	fprintf(stderr, "%s() called\n", __func__);
}

static void
on_exit_func(int exit_status, void *arg)
{
	fprintf(stderr, "%s() called: status=%d, arg=%ld\n",
		__func__, exit_status, (long) arg);
}

int
main(int argc, char *argv[])
{
	if (0 != on_exit(on_exit_func, (void*) 10)) {
		fprintf(stderr, "on_exit() 1\n");
		exit(EXIT_FAILURE);
	}

	if (0 != atexit(atexit_func1)) {
		fprintf(stderr, "atexit() 1\n");
		exit(EXIT_FAILURE);
	}

	if (0 != atexit(atexit_func2)) {
		fprintf(stderr, "atexit() 2\n");
		exit(EXIT_FAILURE);
	}

	if (0 != on_exit(on_exit_func, (void*) 20)) {
		fprintf(stderr, "on_exit 2\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
