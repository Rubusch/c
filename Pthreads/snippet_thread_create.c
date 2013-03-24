// snippet_create.c
/*
  demonstrates thread creation
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

  if(0 != pthread_create(&thread_id, NULL, thread_function, (void*) some_argument)){
    perror("pthread_create() failed");
    exit(EXIT_FAILURE);
  }

  // ...
  exit(SUCCESS);
}


void* thread_function(void* arg)
{
  anytype local_some_argument = (anytype) arg;

  // ... 
  pthread_exit(NULL);
}
