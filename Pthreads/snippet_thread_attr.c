// snippet_threadattr.c
/*
  demonstrates creation and configuration of a thread
//*/

// ...
#include <pthread.h>

// globally declared thread function
void* thread_function(void* arg);
// ...

pthread_t thread_id;
anytype some_argument;
// ...


// the calling function contains..
{
  // ...
  // init the attributes - this might also be possible with 
  // a macro, when the pthread_attr_t variable is global!
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  // set PTHREAD_CREATE_JOINABLE (=default, and therefore no 
  // attr is needed) or PTHREAD_CREATE_DETACHED
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  int return_value=0;
  if(0 != (return_value = pthread_create(&thread_id, NULL, thread_function, (void*) some_argument))){
    perror("pthread_create() failed");
    exit(EXIT_FAILURE);
  }

  // ...
  // also to change the detached state from detached to joinable, 
  // first the attribute needs to be destroyed
  pthread_attr_destroy(&attr); 
  exit(EXIT_SUCCESS);
}


void* thread_function(void* arg)
{
  anytype local_some_argument = (anytype) arg;

  // ... 
  pthread_exit(NULL);
}
