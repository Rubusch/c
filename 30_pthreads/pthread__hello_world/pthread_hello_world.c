// pthread_hello_world.c
/*
  compile with -lm -pthread as LIBS or e.g.
        ${CC} -o $@ ${OBJECTS} -lm -pthread

  pthread_create(thread, attr, start_function, argument) - creates a new thread
and executes it pthread_exit(status)       - the exit function when working with
pthreads pthread_attr_init(attr)    - initializes the thread attribute object
  pthread_attr_destroy(attr) - destroys the thread attribute object
//*/

#include <stdint.h> /* intptr_t */
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define NUM_THREADS 5

void *print_hello(void *thread_id)
{
	int tid = (int)(intptr_t)thread_id;
	printf("Hello World! It's me, thread #%i!\n", tid);

	// shutdown thread
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	// define array of pthreads
	pthread_t threads[NUM_THREADS];

	int return_code = 0, cnt = 0;
	for (cnt = 0; cnt < NUM_THREADS; ++cnt) {
		printf("in main: creating thread %d\n", cnt);

		// create pthread and execute function
		return_code = pthread_create(&threads[cnt], NULL, print_hello,
					     (void *)(intptr_t)cnt);

		if (return_code) {
			printf("ERROR; return code from pthread_create() is %d\n",
			       return_code);
			exit(-1);
		}
	}

	// shutdown all threads in case of error
	pthread_exit(NULL);
}
