// deadlock.c
/*
  demonstrates a deadlock using POSIX threads:
  at pressing enter - after a while and numerous ENTERs the proggy runs into a deadlock,
  since the mutex is still locked by the counter thread, the main thread put the couter thread into "suspend" 
  and (because the mutex already is locked), the main thread is not able to lock the mutex for it.
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

// thread function
void* counter(void*);

int count;

// mutex
pthread_mutex_t lock_count;

// replacement for thr_suspend()
int suspend;


int main()
{
  char str[80];

  // thr_suspend() replacement
  suspend = 0;

  // init mutex
  pthread_mutex_init(&lock_count, NULL);

  // init thread attributes
  pthread_attr_t attr_counter;
  pthread_attr_init(&attr_counter);
  pthread_attr_setdetachstate(&attr_counter, PTHREAD_CREATE_DETACHED);
  
  // create thread (PTHREAD_CREATE_DETACHED)
  pthread_t tid_counter;
  if(0 != pthread_create(&tid_counter, &attr_counter, counter,(void*) 0)){
    perror("pthread_create failed");
    exit(EXIT_FAILURE);
  }

  while(1) {
    // enter a string
    puts("press ENTER");
    fgets(str, 80, stdin);

    // replace: thr_continue();
    suspend = 1;

    // lock
    pthread_mutex_lock(&lock_count);

    printf("\n\nCOUNT = %d\n\n", count);

    // unlock
    pthread_mutex_unlock(&lock_count);

    suspend = 0;    
  }
  
  // destroy thread attribute
  pthread_mutex_destroy(&lock_count);
  pthread_attr_destroy(&attr_counter);
  puts("READY.");
  return(0);
}


/*
  thread function
//*/
void* counter(void* arg)
{
  while(1){ 
    // locking the mutex before "suspend"-if
    pthread_mutex_lock(&lock_count);

    printf("."); 
    fflush(stdout);
    int idx;
    
    if(!suspend){ // this happens with a locked mutex -> dangerous, it will end up in a dead lock after a while!
      ++count;    
      for (idx = 0; idx < 500; ++idx) // the lower the number, the easier to run into a deadlock here 
	;
    
      // unlock - only within the "suspend"-if
      pthread_mutex_unlock(&lock_count); 
      
      for (idx = 0; idx < 500; ++idx)
	;
    }
  }
  pthread_exit((void*) 0);  
}

