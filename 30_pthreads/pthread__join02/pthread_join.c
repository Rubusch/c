/*
   	pthread_join.c
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#define NTHREADS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void*
thread_function(void *dummy)
{
	printf("thread number %d\n", abs(pthread_self()));
	pthread_mutex_lock(&mutex);
	counter++;
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

int
main(int argc, char *argv[])
{
	pthread_t thread_id[NTHREADS];
	int idx = 0;

	for (idx = 0; idx < NTHREADS; ++idx) {
		if (0 != pthread_create(&thread_id[idx], NULL, thread_function,
					NULL)) {
			perror("pthread_create() failed");
			exit(1);
		}
	}
	for (idx = 0; idx < NTHREADS; ++idx) {
		if (0 != pthread_join(thread_id[idx], NULL)) {
			perror("pthread_join() failed");
			exit(1);
		}
	}

	// threads are finished -> print counter
	printf("print final counter: %d\n", counter);

	pthread_exit(NULL);
}

