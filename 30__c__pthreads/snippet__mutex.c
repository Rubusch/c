// snippet_mutex.c
/*
  demonstrates creation and usage of a mutex

  mutex / locking is active wait!
//*/

// ...
#include <pthread.h>

// either globally declared mutexes:
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// ...or, e.g. in a struct, if the
// struct is the ressource to which the success needs to be
// synchronized
struct {
	pthread_mutex_t data_lock;
	// ...
} some_data;
// ...

void *some_thread_function(void *);
// ...

// some calling function that creates the thread
{
	some_data data;
	// ...
	if (0 != pthread_mutex_init((void *)&data.data_lock[idx], NULL)) {
		perror("pthread_mutex_lock failed");
		exit(EXIT_FAILURE);
	}
	// ...

	pthread_t thread_id;
	if (0 != pthread_create(&thread_id, NULL, some_thread_function, NULL)) {
		perror("MAIN: pthread_create failed");
		break;
	}
	// ...

	// cleanup when done
	pthread_mutex_destroy(&data.data_lock);
	exit(EXIT_SUCCESS);
}

void *some_thread_function(void *some_arg)
{
	// ...
	pthread_mutex_lock(&data.data_lock);
	// do something
	pthread_mutex_unlock(&data.data_lock);
	// ...
}
