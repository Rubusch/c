/*
  cleanup handler

  usage:
  $ ./cleanup.elf foo
    thread_func(): allocated memory at 0xb64005f8
    main() about to signal condition variable
    thread_func(): condition wait loop completed     <---
    cleanup_handler(): freeing block at 0xb64005f8
    cleanup_handler(): unlocking mutex
    main(): thread terminated normally
    READY.


  The main program creates a thread whose first actions are to
  allocate a block of memory whose location is stored in buf, and then
  lock the mutex mtx. Since the thread may be canceled, it uses
  pthread_cleanup_push() to install a cleanup handler that is called
  with the address stored in buf. If it is invoked, the cleanup
  handler deallocates the freed memory and unlocks the mutex.

  The thread then enters a loop waiting for the condition variable
  cond to be signa led. This loop will terminate in one of two ways,
  depending on whether the program is supplied with a command-line
  argument:

  - If no command-line argument is supplied, the thread is canceled by
    main(). In this case, cancellation will occur at the call to
    pthread_cond_wait().

  - If a command-line argument is supplied, the condition variable is
    signaled after the associated global variable, glob, is first set
    to a nonzero value. In this case, the thread falls through to
    execute pthread_cleanup_pop(), which, given a nonzero argument,
    also causes the cleanup handler to be invoked.


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 678
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <unistd.h>

// static initializers..
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int glob = 0;

static void         // free memory pointed to by 'arg' and unlock mutex
cleanup_handler(void *arg)
{
	int ret;

	fprintf(stderr, "%s(): freeing block at %p\n", __func__, arg);
	free(arg);

	fprintf(stderr, "%s(): unlocking mutex\n", __func__);
	ret = pthread_mutex_unlock(&mtx);
	if (0 != ret) {
		fprintf(stderr, "pthread_mutex_unlock() failed\n");
		_exit(ret);
	}
}

static void*
thread_func(void *arg)
{
	int ret;
	void *buf = NULL;      // buffer allocated by thread

	buf = malloc(0x10000); // not a cancellation point
	fprintf(stderr, "%s(): allocated memory at %p\n",
		__func__, buf);

	ret = pthread_mutex_lock(&mtx); // not a cancellation point
	if (0 != ret) {
		fprintf(stderr, "pthread_mutex_lock() failed\n");
		pthread_exit(NULL);
	}

	pthread_cleanup_push(cleanup_handler, buf);

	while (0 == glob) {
		ret = pthread_cond_wait(&cond, &mtx);
		if (0 != ret) {
			fprintf(stderr, "pthread_cond_wait() failed\n");
			pthread_exit(NULL);
		}
	}

	fprintf(stderr, "%s(): condition wait loop completed\n",
		__func__);

	pthread_cleanup_pop(1);  // executes cleanup handler
	return NULL;
}

int
main(int argc, char *argv[])
{
	pthread_t thr;
	void *res;
	int ret;

	ret = pthread_create(&thr, NULL, thread_func, NULL);
	if (0 != ret) {
		fprintf(stderr, "pthread_create() failed\n");
		exit(EXIT_FAILURE);
	}

	sleep(2);   // give thread chance to get started

	if (1 == argc) { // cancel thread
		fprintf(stderr, "%s(): about to cancel thread...\n", __func__);
		ret = pthread_cancel(thr);
		if (0 != ret) {
			fprintf(stderr, "pthread_cancel() failed\n");
			exit(EXIT_FAILURE);
		}
	} else {    // signal condition variable
		fprintf(stderr, "%s() about to signal condition variable\n",
			__func__);
		glob = 1;
		ret = pthread_cond_signal(&cond);
		if (0 != ret) {
			fprintf(stderr, "pthread_cond_signal() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	ret = pthread_join(thr, &res);
	if (0 != ret) {
		fprintf(stderr, "pthread_join() failed\n");
		exit(EXIT_FAILURE);
	}

	if (res == PTHREAD_CANCELED) {
		fprintf(stderr, "%s(): thread was canceled\n", __func__);
	} else {
		fprintf(stderr, "%s(): thread terminated normally\n", __func__);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
