// manythreads.c
/*
  demonstrates many threads running at same time

  start the program with the given number of threads to run
  each thread will wait on the previous thread,
  an output will be demonstrated when the threads exit
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

// function prototypes and global varaibles
void *thr_sub(void *);
pthread_mutex_t lock;

/*
  main
//*/
int main(int argc, char **argv)
{
	/*
     check to see if user passed an argument
     -- if so, set the number of threads to the value
     passed to the program
  */
	int thr_count = 100;
	if (argc == 2) {
		thr_count = atoi(argv[1]);
	}
	printf("Creating %d threads...\n", thr_count);

	/*
     lock the mutex variable -- this mutex is being used to
     keep all the other threads created from proceeding
  */
	pthread_mutex_lock(&lock);

	/*
     create all the threads -- Note that a specific stack size is
     given.  Since the created threads will not use all of the
     default stack size, we can save memory by reducing the threads'
     stack size
  */
	pthread_t threads[thr_count];
	int idx;
	for (idx = 0; idx < thr_count; ++idx) {
		if (0 != pthread_create((pthread_t *)&threads[idx], NULL,
					thr_sub, (void *)2048)) {
			perror("pthread create failed");
			break;
		}
	}

	printf("%d threads have been created and are running!\n", idx);
	printf("Press <return> to join all the threads...\n");

	// wait till user presses return, then join all the threads
	fgetc(stdin);

	printf("Joining %d threads...\n", thr_count);

	// now unlock the mutex variable, to let all the threads proceed
	pthread_mutex_unlock(&lock);

	// join the threads
	for (idx = 0; idx < thr_count; ++idx) {
		pthread_join(threads[idx], NULL);
	}

	printf("All %d threads have been joined, exiting...\n", thr_count);
	exit(EXIT_SUCCESS);
}

/*
   The routine that is executed by the created threads
*/
void *thr_sub(void *arg)
{
	/*
     try to lock the mutex variable -- since the main thread has
     locked the mutex before the threads were created, this thread
     will block until the main thread unlock the mutex
  */

	pthread_mutex_lock(&lock);

	printf("Thread %lu is exiting...\n", (unsigned long)pthread_self());

	/* unlock the mutex to allow another thread to proceed */
	pthread_mutex_unlock(&lock);

	/* exit the thread */
	return ((void *)0);
}
