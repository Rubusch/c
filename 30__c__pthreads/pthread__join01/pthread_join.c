// pthread_join.c
/*
  create a thread either "joinable" (synchronized) or "detachable" (independent)

  - better to do so, even if it won't be used later on
  - a "joinable" thread can be "detached" but not the other way round

  typical 4 step process to declare a thread in case of detach:
  1. declare a pthread attribute variable of the type pthread_attr_t
  2. initialize the attribute variable with pthread_attr_init()
  3. set the attribute "detachestate" with pthread_attr_setdetachstate() to
PTHREAD_CREATE_JOINABLE
  4. when done, free library resources used by the attribute with
pthread_attr_destroy()


  pthread_join(thread_id, detachstate)           - joins a thread
(synchronization) pthread_detach(thread_id, detachstate)         - detaches a
thread (independance) pthread_attr_setdetachstate(attr, detachstate) - sets the
status of a thread "detachable"/"joinable" pthread_attr_getdetachstate(attr,
detachstate) - returns the detachstate of a thread, "detachable"/"joinable"
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#define NUM_THREADS 5

extern long int random(void);

void *busy_work(void *dummy)
{
	unsigned int idx = 0;
	double result = 0.0;
	for (idx = 0; idx < 100000; ++idx) {
		result += (double)random();
	}

	printf("result = %e\n", result);
	pthread_exit((void *)0);
}

int main(int argc, char **argv)
{
	// 1. attr - prepare thread to be joinable/detachable
	pthread_attr_t attr;
	pthread_t thread[NUM_THREADS];
	int return_code = 0, cnt = 0;

	// status variable for the join status
	void *status;

	// 2. pthread_attr_init(attr) - initialize the variable
	pthread_attr_init(&attr);

	// 3. pthread_attr_setdetachstate() - set the "detachstate" to joinable
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// create the threads with busy_work()
	for (cnt = 0; cnt < NUM_THREADS; ++cnt) {
		printf("creating thread %d\n", cnt);
		if (0 != (return_code = pthread_create(&thread[cnt], &attr,
						       busy_work, NULL))) {
			fprintf(stderr,
				"ERROR; return code from pthread_create() is %d\n",
				return_code);
			exit(1);
		}
	}

	// 4. pthread_attr_destry(&attr) - free attribute and wait for the other
	// threads, when done
	pthread_attr_destroy(&attr);

	// prepare join for the threads
	for (cnt = 0; cnt < NUM_THREADS; ++cnt) {
		// make the main thread wait on the joined thread!
		if (0 != (return_code = pthread_join(thread[cnt], &status))) {
			fprintf(stderr,
				"ERROR; return code from pthread_join() is %d\n",
				return_code);
			exit(1);
		}
		printf("completed joint with thread %d status = %ld\n", cnt,
		       (long)status);
	}

	pthread_exit(NULL);
}
