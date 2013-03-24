// pthread_passing_arguments.c
/*

//*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

#define NUM_THREADS 5

// new struct to pass
struct thread_data{
  int thread_id;
  int sum;
  char message[20];
};

// declare a global array of threads
struct thread_data thread_data_array[NUM_THREADS];


void* print_hello(void* thread_arg)
{
  //  declared now as: struct thread_data and init the local variables
  struct thread_data* my_data = (struct thread_data*) thread_arg;
  int task_id = my_data->thread_id;
  int sum = my_data->sum;
  char* hello_msg = my_data->message;

  printf("%s, the task_id is %i, the sum is %i!\n", hello_msg, task_id, sum);

  pthread_exit(NULL);
}


int main(int argc, char** argv)
{
  pthread_t threads[NUM_THREADS];
  int return_code=0, cnt=0, sum=0;
  for(cnt=0; cnt<NUM_THREADS; ++cnt,  sum+=cnt){
    printf("in main: creating thread %d\n", cnt);

    // initializing the new struct to pass over
    thread_data_array[cnt].thread_id = cnt;
    thread_data_array[cnt].sum = sum;
    strncpy(thread_data_array[cnt].message,"Hello World!", strlen("Hello World!"));

    if(0 != (return_code = pthread_create(&threads[cnt], NULL, print_hello, (void*) &thread_data_array[cnt]))){
      fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
  }
  pthread_exit(NULL);
}
