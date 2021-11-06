// pthread_mutex.c
/*
  MUTEXES (used generally for synchronizing two threads, can provoke deadlocks
with more threads)



  pthread_self() - returns the unique, system assigned thread ID of the calling
thread

  pthread_equal(thread_id1, thread_id2) - compares two thread IDs

  pthread_once(once_control, init_function) - executes the given init_function
exactly once in a process once_control is a synchronization control structure
that requires initialization prior to calling pthread_once(), e.g.:
pthread_once_t once_control = PTHREAD_ONCE_INIT;

  pthread_yield() - forces the calling thread to relinquish use of its processor
and to wait in the run queue before it is scheduled again

  pthread_mutex_init(mutex, attr) - creates a mutex object dynamically
  pthread_mutex_destroy(mutex)    - destroys a mutex object dynamically
  pthread_mutexattr_init(attr)    - creates a mutexattribute object dynamically
  pthread_mutexattr_destroy(attr) - destroys a mutexattribute object dynamically


  Mutex:

  1. create and initialize a mutex varable
  2. several threads attempt to lock the mutex
  3. only one succeeds and that thread owns the mutex
  4. the owner thread performs  some set of actions
  5. the owner unlocks the mutex
  6. another thread acquires the mutex and repeats the process
  7. finally the mutex is destroyed

  mutex variables must be declared with type pthrea_mutex_t and must be
initialized befofe they can be used, mutex variables initially are UNLOCKED

  - mutex variables can be declared statically
  pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

  - mutex variables can be declared dynamically using pthread_mutex_init(attr);


  Mutexattribute:

  pthread_mutexattr_t, can be initialized by
  - NULL (defaults)
  - protocol (specifies the protocol used to prevent priority inversions for a
mutex)
  - prioceiling (specifies the priority ceiling of a mutex)
  - process-shared (specifies the process sharing of a mutex)


  pthread_mutex_lock(mutex)    - used by a thread to acquire a lock on the
specified mutex variable this call will block other calls for the same mutex

  pthread_mutex_trylock(mutex) - if the mutex is already locked this call will
return with a busy error code

  pthread_mutex_unlock(mutex)  - unlocks a mutex if called by the owner of the
mutex, returns an error if the mutex is owned by another thread or if the mutex
already was unlocked
//*/

#include <stdint.h> /* intptr_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

/*
  structure contains necessary information to allow the function dotprod() to
access its input data and place its output into the structure
//*/
typedef struct {
	double *a;
	double *b;
	double sum; // to be changed -> mutex
	int veclen;
} DOTDATA;

/*
  define globally accessible variables to share and a mutex
//*/
#define NUMTHRDS 4
#define VECLEN 100

// the global datastructure
DOTDATA dotstr;

// a set of threads
pthread_t callThd[NUMTHRDS];

// the MUTEX variable:
pthread_mutex_t mutexsum;

/*
  the thread function, all input to this function is obtained from a structure
of type DOTDATA and all output from this function is written into this
structure. when a thread is created we pas a single argument to the activated
function - typically this argument is a thread number
//*/
void *dotprod(void *arg)
{
	// local variables
	int idx = 0, start = 0, end = 0, offset = 0, len = 0;
	double mysum = 0.0, *xVal = NULL, *yVal = NULL;

	offset = (int)(intptr_t)arg;
	len = dotstr.veclen;
	start = offset * len;
	end = start + len;
	xVal = dotstr.a;
	yVal = dotstr.b;

	/*
    perform the dot product and assign result to the appropriate variable in the
  structure
  //*/
	mysum = 0;
	for (idx = start; idx < end; ++idx) {
		mysum += (xVal[idx] * yVal[idx]);
	}

	/*
    lock the mutex prior to updating the value in the shared structure and
  unlock it upon updating
  //*/
	pthread_mutex_lock(&mutexsum);
	dotstr.sum += mysum;
	pthread_mutex_unlock(&mutexsum);
	pthread_exit((void *)0);
}

/*
  the main program creates threads which do all the work and then print out
result upon completion
//*/
int main(int argc, char **argv)
{
	int idx;
	double *aVal = NULL, *bVal = NULL;
	void *status = NULL;
	pthread_attr_t attr;

	// alloc memory for the vals
	if (NULL ==
	    (aVal = (double *)malloc(NUMTHRDS * VECLEN * sizeof(*aVal)))) {
		perror("malloc() failed");
		exit(1);
	}
	if (NULL ==
	    (bVal = (double *)malloc(NUMTHRDS * VECLEN * sizeof(*bVal)))) {
		perror("malloc() failed");
		exit(1);
	}

	// init the vals
	for (idx = 0; idx < VECLEN * NUMTHRDS; ++idx) {
		aVal[idx] = 1.0;
		bVal[idx] = aVal[idx];
	}

	dotstr.veclen = VECLEN;
	dotstr.a = aVal;
	dotstr.b = bVal;
	dotstr.sum = 0;

	pthread_mutex_init(&mutexsum, NULL);

	// creat threads to perform the dotproduct
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	for (idx = 0; idx < NUMTHRDS; ++idx) {
		/*
      each thread works on a different set of data, the offset is specified by
    idx, the size of data for each thread is indicated by VECLEN
    //*/
		pthread_create(&callThd[idx], &attr, dotprod,
			       (void *)(intptr_t)idx);
	}
	pthread_attr_destroy(&attr);

	/*
    wait on the other threads
  //*/
	for (idx = 0; idx < NUMTHRDS; ++idx) {
		pthread_join(callThd[idx], &status);
	}

	/*
    after joining, print out hte results and clean up
  //*/
	printf("sum = %f\n", dotstr.sum);
	free(aVal);
	free(bVal);
	pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
}
