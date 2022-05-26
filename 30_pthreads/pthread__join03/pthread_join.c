/*
   	pthread_join.c

  usage:
  $ ./pthread_join.elf 1 1 2 3 3
    thread 1 terminating
    thread 0 terminating
    reaped thread 0 (nlive=4)
    reaped thread 1 (nlive=3)
    thread 2 terminating
    reaped thread 2 (nlive=2)
    thread 3 terminating
    thread 4 terminating
    reaped thread 3 (nlive=1)
    reaped thread 4 (nlive=0)
    READY.


  The program creates one thread for each of its command-line
  arguments. Each thread sleeps for the number of seconds specified in
  the corresponding command-line argument and then terminates. the
  sleep interval is our means of simulating the idea of a thread that
  does work for a period of time.

  The program maintains a set of global variables recording
  invormation about all of the threads that have been reated. For each
  thread, an element in the global thread array records the ID of the
  thread (the tid field) and its current state (the state field). The
  state field has one of the following values: TS_ALIVE, meaning the
  thread is alive; TS_TERMINATED, meaning the thread hasterminated but
  not yet been joined; or TS_JOINED, meaning the thread has terminated
  and been joined.


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 649
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include "tlpi/get_num.h"

static pthread_cond_t thread_died = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;
// protects all of the following global variables

static int nthreads = 0;      // total number of threads created
static int nlive = 0;         // total number of threads still alive
 			      // or terminated byt not yet joined
static int nunjoined = 0;     // number of terminated therads that
			      // have not yet been joined
enum tstate {
	TS_ALIVE,
	TS_TERMINATED,
	TS_JOINED
};

static struct {
	pthread_t tid;        // info about each thread
	enum tstate state;    // ID of this thread
	int sleep_time;       // thread state (TS_* constants above)
} *thread;                    // number seconds to alive before
			      // terminating

static void*
thread_func(void *arg)
{
	int idx = (int) arg;
	int ret;

	sleep(thread[idx].sleep_time);  // simulate doing some work
	fprintf(stderr, "thread %d terminating\n", idx);

	ret = pthread_mutex_lock(&thread_mutex);
	if (0 != ret) {
		fprintf(stderr, "pthread_mutex_lock() failed\n");
		pthread_exit(NULL);
	}

	nunjoined++;
	thread[idx].state = TS_TERMINATED;

	ret = pthread_mutex_unlock(&thread_mutex);
	if (0 != ret) {
		fprintf(stderr, "pthread_mutex_unlock() failed\n");
		pthread_exit(NULL);
	}

	ret = pthread_cond_signal(&thread_died);
	if (0 != ret) {
		fprintf(stderr, "pthread_cond_signal() failed\n");
		pthread_exit(NULL);
	}

	return NULL;
}

int
main(int argc, char *argv[])
{
	int ret, idx;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s nsecs...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	thread = calloc(argc - 1, sizeof(*thread));
	if (NULL == thread) {
		fprintf(stderr, "calloc() failed\n");
		exit(EXIT_FAILURE);
	}

	// create all threads

	for (idx=0; idx < argc - 1; idx++) {
		thread[idx].sleep_time = get_int(argv[idx + 1], GN_NONNEG, NULL);
		thread[idx].state = TS_ALIVE;
		ret = pthread_create(&thread[idx].tid, NULL, thread_func, (void*) idx);
		if (0 != ret) {
			fprintf(stderr, "pthread_create() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	nthreads = argc - 1;
	nlive = nthreads;

	// join with terminated threads

	while (0 < nlive) {
		ret = pthread_mutex_lock(&thread_mutex);
		if (0 != ret) {
			fprintf(stderr, "pthread_mutex_lock() failed\n");
			exit(EXIT_FAILURE);
		}

		while (0 == nunjoined) {
			ret = pthread_cond_wait(&thread_died, &thread_mutex);
			if (0 != ret) {
				fprintf(stderr, "pthread_cond_wait() failed\n");
				exit(EXIT_FAILURE);
			}
		}

		for (idx = 0; idx < nthreads; idx++) {
			if (thread[idx].state == TS_TERMINATED) {
				ret = pthread_join(thread[idx].tid, NULL);
				if (0 != ret) {
					fprintf(stderr, "pthread_join() failed\n");
					exit(EXIT_FAILURE);
				}

				thread[idx].state = TS_JOINED;
				nlive--;
				nunjoined--;

				fprintf(stderr, "reaped thread %d (nlive=%d)\n", idx, nlive);
			}
		}

		ret = pthread_mutex_unlock(&thread_mutex);
		if (0 != ret) {
			fprintf(stderr, "pthread_mutex_unlock() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "READY.\n");
	pthread_exit(NULL);
}
