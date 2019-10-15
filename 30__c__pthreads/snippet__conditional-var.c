// snippet_conditional_var.c
/*
  demonstrates the usage of a conditional variable

  conditional variables are passive wait!
//*/

// ...
#include <pthread.h>

// ...
pthread_cond_t cv_suspend;

// e.g. init macro for global mutexes with default values
pthread_mutex_t mx_suspend = PTHREAD_MUTEX_INITIALIZER;
// ...

void *thread_to_be_suspended(void *);
// ...


// some calling function that creates the thread
{
  // conditional variable attribute, here as local variable
  pthread_condattr_t attr_cv_suspend;
  pthread_condattr_init(&attr_cv_suspend);
  pthread_condattr_setpshared(&attr_cv_suspend, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&cv_suspend, &attr_cv_suspend);

  // creation
  pthread_t thread_id;
  if (0 != pthread_create(&thread_id, NULL, thread_to_be_suspended, NULL)) {
    perror("pthread_create() failed");
    break;
  }
  // ...

  // cleanup when done
  pthread_cond_destroy(&cv_suspend);
  pthread_mutex_destroy(&mx_suspend);
  pthread_condattr_destroy(&attr_cv_suspend);

  exit(EXIT_SUCCESS);
}


void *thread_to_be_suspended(void *some_arg)
{
  // ...
  pthread_mutex_lock(&mx_suspend);
  // by calling this function this thread goes to sleep,
  // the conditional variable stayes protected by mutexes
  pthread_cond_wait(&cv_suspend, &mx_suspend);
  pthread_mutex_unlock(&mx_suspend);
  // ...
}


// some function to awake the suspended thread
{
  // ...
  // awake suspended thread
  pthread_cond_signal(&cv_suspend);
  // ...
}
