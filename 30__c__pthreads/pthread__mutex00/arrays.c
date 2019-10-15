// Arrays.c
/*
  demonstrates the protection of a common data structure, used by several
threads

  take a look on the order in which the working threads are handling the data
structure
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <stdint.h> /* intptr_t */
#include <unistd.h>

extern int pthread_setconcurrency(int);
extern int pthread_getconcurrency();


// array data structure
struct {
  pthread_mutex_t data_lock[5];
  int int_val[5];
  double double_val[5];
} data;


// thread
void *add_to_value();


int main()
{
  puts("arrays - POSIX version");

  // 5 threads
  pthread_t thr[5];

  // init
  puts("init..");
  int idx = 0;
  for (idx = 0; idx < 5; ++idx) {
    if (0 != pthread_mutex_init(( void * )&data.data_lock[idx], NULL)) {
      perror("pthread_mutex_lock failed");
      exit(EXIT_FAILURE);
    }
    data.int_val[idx] = 0;
    data.double_val[idx] = 0.0;
  }

  // set concurrency and create the threads
  printf("the current level is %d set the concurrency level to 4\n",
         pthread_getconcurrency());
  pthread_setconcurrency(4);

  // create threads
  puts("creating threads");
  for (idx = 0; idx < 5; ++idx) { // safe cast: int -> intptr_t -> void*
    if (0 != pthread_create(( void * )&thr[idx], NULL, add_to_value,
                            ( void * )((intptr_t)(2 * idx)))) {
      perror("pthread_create failed:");
      exit(EXIT_FAILURE);
    }
  }

  // wait till all threads have finished
  puts("joining threads");
  for (idx = 0; idx < 5; ++idx) {
    int *status = 0;
    pthread_join(thr[idx], ( void * )&status);
    if (status == 0) {
      fprintf(stderr, "ERROR: thread %d failed\n", idx);
      exit(EXIT_FAILURE);
    }
  }

  // print the results
  printf("Final Values.....\n");
  for (idx = 0; idx < 5; ++idx) {
    printf("integer value[%d] =\t%d\n", idx, data.int_val[idx]);
    printf("double value[%d] =\t%.1f\n\n", idx, data.double_val[idx]);
  }

  // clean up
  puts("clean up..");
  for (idx = 0; idx < 5; ++idx) {
    pthread_mutex_destroy(( void * )&data.data_lock[idx]);
  }
  puts("READY.");
  exit(EXIT_SUCCESS);
}


/*
   thread implementation
//*/
void *add_to_value(void *arg)
{
  // safe cast: void* -> intptr_t -> int
  int inval = ( int )( intptr_t )arg;

  printf("\nthread %d: was called..\nlock\taction\n", (inval / 2));

  /*
    set the mutex on writing into the data structure
    set a value to both structures and unlock - repeat
    this procedure 5 times (at least once per entry)

    observe the order of the working threads on the structure in the output!
  //*/
  int idx = 0;
  for (idx = 0; idx < 5; ++idx) {
    pthread_mutex_lock(&data.data_lock[idx % 5]);

    printf("\n%d -\tthread %d: locked!\n", (inval / 2), (inval / 2));

    printf("%d -\tidx(%d): %d += %d ", (inval / 2), (idx % 5),
           data.int_val[idx % 5], inval);
    data.int_val[idx % 5] += inval;
    printf("= %d\n", data.int_val[idx % 5]);

    printf("%d -\tidx(%d): %.1f += %.1f ", (inval / 2), (idx % 5),
           data.double_val[idx % 5], (( double )1.5 * inval));
    data.double_val[idx % 5] += ( double )1.5 * inval;
    printf("= %.1f\n", data.double_val[idx % 5]);

    printf("%d -\tthread %d: unlockes!\n", (inval / 2), (inval / 2));
    pthread_mutex_unlock(&data.data_lock[idx % 5]);
  }

  printf("%d -\tthread %d: exits\n", (inval / 2), (inval / 2));
  pthread_exit(( void * )5);
}
