// pthread_stack_management.c
/*
  pthread_attr_getstacksize(attr, stacksize) - returns the stacksize
  pthread_attr_setstacksize(attr, stacksize) - sets the stacksize
  pthread_attr_getstackaddr(attr, stacksize) - returns the stackaddress
  pthread_attr_setstackaddr(attr, stacksize) - sets the stackaddr
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#define NUM_THREADS 4
#define NUM 1000
#define MEGEXTRA 100000

pthread_attr_t attr;


void *do_work(void *thread_id)
{
  double arr[NUM][NUM];
  int row = 0, col = 0;
  unsigned long tid = ( unsigned long )thread_id;
  size_t mystacksize = 0;

  memset(arr, 0, NUM * NUM);

  // get information about the stacksize
  pthread_attr_getstacksize(&attr, &mystacksize);

  printf("thread %lu: stack size = %lu bytes\n", tid, mystacksize);
  for (col = 0; col < NUM; ++col) {
    for (row = 0; row < NUM; ++row) {
      arr[col][row] = ((row * col) / 3.452) + (NUM - col);
    }
  }
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  pthread_t threads[NUM_THREADS];
  size_t stacksize = 0;
  int rc = 0;
  unsigned long cnt = 0;

  pthread_attr_init(&attr);

  // get general stacksize
  pthread_attr_getstacksize(&attr, &stacksize);

  printf("default stack size = %li bytes\n", ( long int )stacksize);
  stacksize = sizeof(double) * NUM * NUM + MEGEXTRA;

  printf("amount of stack needed per thread = %li bytes\n",
         ( long int )stacksize);
  for (cnt = 0; cnt < NUM_THREADS; ++cnt) {
    if (0 !=
        (rc = pthread_create(&threads[cnt], &attr, do_work, ( void * )cnt))) {
      fprintf(stderr, "ERROR; pthread_create() - %d\n", rc);
      exit(-1);
    }
  }
  printf("created %lu threads\n", cnt);
  pthread_exit(NULL);
}
