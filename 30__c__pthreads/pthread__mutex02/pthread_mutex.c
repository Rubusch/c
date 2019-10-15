// pthread_mutex.c
/*

//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

void *thread_function();

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

int main(int argc, char **argv)
{
  int rc1 = 0, rc2 = 0;
  pthread_t thread1, thread2;

  // create independent threads each of which will execute the thread_function()
  if (0 != (rc1 = pthread_create(&thread1, NULL, &thread_function, NULL))) {
    perror("pthread_create() failed");
    exit(1);
  }
  if (0 != (rc2 = pthread_create(&thread2, NULL, &thread_function, NULL))) {
    perror("pthread_create() failed");
    exit(1);
  }

  // wait until threads are completed
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  pthread_exit(NULL);
}


void *thread_function()
{
  pthread_mutex_lock(&mutex);
  ++counter;
  printf("counter value: %d\n", counter);
  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}
