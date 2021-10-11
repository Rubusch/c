//  pthread_passing_arguments.c
/*
  shows a incorrect pass to a thread:

  passing by reference of variables leads to errors since they are
  not synchronized!!!
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#define NUM_THREADS 5

void *print_hello(void *thread_id)
{
	unsigned long tid = (unsigned long)thread_id;
	printf("Hello World! It's me, thread %lu!\n", tid);
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t threads[NUM_THREADS];
	int return_code = 0, cnt = 0;
	for (cnt = 0; cnt < NUM_THREADS; ++cnt) {
		printf("in main: creating thread %d\n", cnt);

		// incorrect pass - cnt will be modified by parent, a pass
		// by reference leads to incorrect results
		if (0 !=
		    (return_code = pthread_create(&threads[cnt], NULL,
						  print_hello, (void *)&cnt))) {
			fprintf(stderr,
				"ERROR; return code from pthread_create() is %d\n",
				return_code);
			exit(-1);
		}
	}

	pthread_exit(NULL);
}
