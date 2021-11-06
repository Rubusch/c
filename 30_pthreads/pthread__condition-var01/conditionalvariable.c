// pthread_condition_var.c
/*
  CONDITION VARIABLES


  condition variables are declared with pthread_cond_t and must be initialized
before usage, either statically (outside of a function): pthread_cond_t myconvar
= PTHREAD_COND_INITIALIZER;

  or dynamically:
      pthread_cond_init()


  pthread_cond_init(condition, attr) - create a condition variable

  pthread_cond_destroy(condition)    - destroy a condition variable

  pthread_condattr_init(attr)        - create condition attributes

  pthread_contaddr_destroy(attr)     - destroy condition attributes

  pthread_cond_wait(condition, mutex) - blocks the calling thread until the
specified condition is signalled, the function should only be called iwth a
locked mutex. The mutex will be released automatically while the condition is
waiting. After signal is received and the thread is awakened, mutex will be
automatically locked for use by the thread. The programmer is then responsible
for unlocking the mutex when the thread is finished with it

  pthread_cond_signal(condition) - is used to signal or awake another thread
which is waiting on the condition variable. It should be called after mutex is
locked and must unlock mutex in order for pthread_cond_wait() routine to
complete

  pthread_cond_broadcast(condition) - should be used instead of
pthread_cond_signal() if more than one thread is in a blocking wait state


  the example shows the use of dynamic joins and mutex-es
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

int count = 0;
int thread_ids[NUM_THREADS] = { 0, 1, 2 };

// declare mutex
pthread_mutex_t mx_count;

// declare cond
pthread_cond_t cv_count_threshold;

extern long int random();

/*
  check the value of the count and signal waiting thread when condition is
reached. Note: this occurs while mutex is locked!
//*/
void *inc_count(void *idp)
{
	int idx1 = 0, idx2 = 0;
	double result = 0.0;
	int *my_id = idp;

	for (idx1 = 0; idx1 < TCOUNT; ++idx1) {
		// lock the mx
		pthread_mutex_lock(&mx_count);
		++count;

		if (count == COUNT_LIMIT) {
			// send a conditional signal if count at limit
			pthread_cond_signal(&cv_count_threshold);
			printf("inc_count(): thread %d, count = %d threshold reached.\n",
			       *my_id, count);
		}
		printf("inc_count(): thread %d, count = %d, unlocking mutex\n",
		       *my_id, count);

		// unlock the mx
		pthread_mutex_unlock(&mx_count);

		/*
      do some work so threads can alternate on mutex lock
    //*/
		for (idx2 = 0; idx2 < 1000; ++idx2) {
			result += (double)random();
		}
	}
	pthread_exit(NULL);
}

/*
  lock mutex and wait for signal.
  Note: the pthread_cond_wait function will automatically and atomically unlock
mutex while it waits.

  Also note: that if COUNT_LIMIT is reached before this routine is run by the
waiting thread, the loop will be skipped to prevent pthread_cond_wait from never
returning.
//*/
void *watch_count(void *idp)
{
	int *my_id = idp;
	printf("starting watch_count(): thread %d\n", *my_id);

	// lock the mx
	pthread_mutex_lock(&mx_count);
	if (count < COUNT_LIMIT) {
		pthread_cond_wait(&cv_count_threshold, &mx_count);
		printf("watch_count(): thread %d condition signal received\n",
		       *my_id);
	}
	pthread_mutex_unlock(&mx_count);

	pthread_exit(NULL);
}

/*
  main
//*/
int main(int argc, char **argv)
{
	int idx = 0, rc = 0;
	pthread_t threads[3];
	pthread_attr_t attr;

	// initialize mutex and condition variable objects
	pthread_mutex_init(&mx_count, NULL);

	// for portability, explicitly create threads in a joinable state
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (0 != (rc = pthread_create(&threads[0], &attr, inc_count,
				      (void *)&thread_ids[0]))) {
		fprintf(stderr, "ERROR: %d\n", rc);
		exit(1);
	}
	if (0 != (rc = pthread_create(&threads[1], &attr, inc_count,
				      (void *)&thread_ids[1]))) {
		fprintf(stderr, "ERROR: %d\n", rc);
		exit(1);
	}
	if (0 != (rc = pthread_create(&threads[2], &attr, watch_count,
				      (void *)&thread_ids[2]))) {
		fprintf(stderr, "ERROR: %d\n", rc);
		exit(1);
	}

	// wait for all threads to complete
	for (idx = 0; idx < NUM_THREADS; ++idx) {
		pthread_join(threads[idx], NULL);
	}
	printf("main(): waited on %d threads, done\n", NUM_THREADS);

	// clean up and exit
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mx_count);
	pthread_cond_destroy(&cv_count_threshold);
	pthread_exit(NULL);
}
