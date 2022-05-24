// pthread_mutex.c
/*

//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void*
thread_function()
{
	int ret;

	ret = pthread_mutex_lock(&mutex);
	if (0 != ret) {
		fprintf(stderr, "pthread_mutex_lock() failed\n");
		_exit(EXIT_FAILURE);
	}

	++counter;
	printf("counter value: %d\n", counter);

	ret = pthread_mutex_unlock(&mutex);
	if (0 != ret) {
		fprintf(stderr, "pthread_mutex_unlock() failed\n");
		_exit(EXIT_FAILURE);
	}

	pthread_exit(NULL);
}

int
main(int argc, char* argv[])
{
	int rc1 = 0, rc2 = 0;
	pthread_t thread1, thread2;

	// create independent threads each of which will execute the
	// thread_function()
	rc1 = pthread_create(&thread1, NULL, &thread_function, NULL);
	if (0 != rc1) {
		perror("pthread_create() failed");
		exit(EXIT_FAILURE);
	}

	rc2 = pthread_create(&thread2, NULL, &thread_function, NULL);
	if (0 != rc2) {
		perror("pthread_create() failed");
		exit(EXIT_FAILURE);
	}

	// wait until threads are completed
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	pthread_exit(NULL);
}
