// snippet_join.c
/*
  demonstrates a thread A "joining" thread B,

  this means A will wait on B's execution
//*/


#include <pthread.h>
// ...

void *thread_function(void *);
// ...


// some calling function that creates the thread
{
  pthread_t thread_id;
  pthread_attr_t attr;
  // ...

  // the thread HAS to be joinable!!!
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  // ...

  pthread_t calling_thread_id = pthread_self();
  // ...

  // creation
  if (0 !=
      pthread_create(&thread_id, &attr, thread_function, &calling_thread_id)) {
    perror("MAIN: pthread_create failed");
    break;
  }
  // ...

  // now wait on the execution of the created thread!!!
  int return_status = 0;
  if (0 > pthread_join(thread_id, ( void * )&return_status)) {
    perror("pthread_join failed");
    pthread_exit(( void * )11);
    // in case this already was a thread,
    // just any return code, say, 11 - might also be NULL
  }
  // ...

  // cleanup
  pthread_attr_destroy(&attr);
  exit(EXIT_SUCCESS);
}


void *thread_function(void *some_arg)
{
  // ...
  pthread_exit(( void * )123);
}
