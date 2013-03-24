// pthread_condition_var.c
/*
  demonstrates the use of condition variables - using static join and static mutex-es
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

pthread_mutex_t mutex_count = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_condition = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_condition = PTHREAD_COND_INITIALIZER;

void* function_count1(void*);
void* function_count2(void*);

int count = 0;

#define COUNT_DONE 1000
#define COUNT_HALT1 3
#define COUNT_HALT2 6


int main(int argc, char** argv)
{
  pthread_t thread1, thread2;

  if(0 != pthread_create(&thread1, NULL, function_count1, NULL)){
    perror("1. pthread_create() failed");
    exit(1);
  }
  if(0 != pthread_create(&thread2, NULL, function_count2, NULL)){
    perror("2. pthread_create() failed");
    exit(1);
  }

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  pthread_exit(NULL);
}


void* function_count1(void* dummy)
{
  while(1){
    // mutex_condition -> wait and "while"
    pthread_mutex_lock( &mutex_condition);
    while( (count >= COUNT_HALT1) && (count <= COUNT_HALT2)){
      pthread_cond_wait( &cond_condition, &mutex_condition);
    }
    pthread_mutex_unlock(&mutex_condition);

    // mutex_count
    pthread_mutex_lock(&mutex_count);
    ++count;
    printf("function 1 - counter value function_count1(): %d\n", count);
    pthread_mutex_unlock( &mutex_count);

    // exit
    if(count >= COUNT_DONE) pthread_exit(NULL);
  }
}


void* function_count2(void* dummy)
{
  while(1){
    // mutex_condition -> signal and "if"
    pthread_mutex_lock( &mutex_condition);
    if( (count < COUNT_HALT1) || (count > COUNT_HALT2)){
      pthread_cond_signal( &cond_condition);
    }
    pthread_mutex_unlock(&mutex_condition);

    // mutex_count
    pthread_mutex_lock(&mutex_count);
    ++count;
    printf("function 2 - counter value function_count2(): %d\n", count);
    pthread_mutex_unlock(&mutex_count);

    // exit
    if(count >= COUNT_DONE) pthread_exit(NULL);
  }
}
			 
