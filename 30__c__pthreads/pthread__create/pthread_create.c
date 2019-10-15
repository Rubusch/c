// pthread_create.c
/*
  demonstrates the creation of pthreads
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

void *print_message(void *ptr);

int main(int argc, char **argv)
{
  pthread_t thread1, thread2;
  char message1[] = "Thread 1";
  char message2[] = "Thread 2";
  int iret1 = 0, iret2 = 0;

  // create independent threads each of which will execute function
  if (0 != (iret1 = pthread_create(&thread1, NULL, print_message,
                                   ( void * )message1))) {
    perror("pthread_create() failed");
    exit(1);
  }
  if (0 != (iret2 = pthread_create(&thread2, NULL, print_message,
                                   ( void * )message2))) {
    perror("pthread_create() failed");
    exit(1);
  }

  /*
    wait until threads are complete before main continues, if we don't
    wait we run the risk of executing an exit which will terminate
    the process and all threads before the threads have completed
  //*/
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("thread 1 returns: %d\n", iret1);
  printf("thread 2 returns: %d\n", iret2);

  pthread_exit(NULL);
}


void *print_message(void *ptr)
{
  char *message = ( char * )ptr;
  printf("%s\n", message);

  // use pthread_exit(NULL) individually here!
  pthread_exit(NULL);
}
