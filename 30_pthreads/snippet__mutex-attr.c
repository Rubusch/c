// snippet_mutexattr.c
/*
  demonstrates configuration of a mutex using a mutex attribute

  mutex / locking is active wait!
//*/

// ...
#include <pthread.h>

// either globally declared mutexes:
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// ...or, e.g. in a struct (= the resource)
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
	// e.g. declare mutex attributes as local varaible
	pthread_mutexattr_t mutex_attr;
	pthread_mutexattr_init(&mutex_attr);
	// e.g. set global using PTHREAD_PROCESS_SHARED or PTHREAD_PROCESS_PRIVATE
	pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
	if (0 != pthread_mutex_init((void *)&data.data_lock[idx], mutex_attr)) {
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
	pthread_mutexattr_destroy(&mutex_attr);
	exit(EXIT_SUCCESS);
}

void *some_thread_function(void *some_arg)
{
	// ...
	pthread_mutex_lock(&data.data_lock);
	// do something protected by locking
	pthread_mutex_unlock(&data.data_lock);

	// ...
	pthread_exit(NULL);
}
