// pthread_passing_arguments.c
/*
  same code as pthread_hello_world

 - the pthread_create() routinge permits to pass ONE argument to the thread
start routine.
 - all arguments must be passed by reference and cast to (void*)
//*/

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define NUM_THREADS 5


void *print_hello(void *thread_id)
{
  unsigned long tid = ( unsigned long )thread_id;
  printf("Hello World! It's me, thread %lu!\n", ( unsigned long )tid);

  // shutdown thread
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  pthread_t threads[NUM_THREADS];

  // something to pass to a thread at initiation
  int *taskids[NUM_THREADS];

  int return_code = 0, cnt = 0;
  for (cnt = 0; cnt < NUM_THREADS; ++cnt) {
    if (NULL == (taskids[cnt] = ( int * )malloc(sizeof(int)))) {
      perror("malloc failed");
      exit(1);
    }
    *(taskids[cnt]) = cnt;
    printf("creating thread %d\n", cnt);

    // now passes taskids[] to the thread handling function
    if (0 != (return_code = pthread_create(&threads[cnt], NULL, print_hello,
                                           ( void * )taskids[cnt]))) {
      fprintf(stderr, "ERROR; return code from pthread_create() is %d\n",
              return_code);
      exit(-1);
    }
  }

  pthread_exit(NULL);
}
