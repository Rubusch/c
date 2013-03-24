// cancellation.c
/*
  Chuck-a-luck with threads

  demonstrates the "cancel" command sent to a thread, possible 
  by the POSIX standard

  this can end up in a segfault for still unknown reason!!!
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#include <errno.h> // EBUSY

// defines the number of searching threads
#define NUM_THREADS 25

/*
  hehe..
#undef RAND_MAX
#define RAND_MAX 9999
//*/

// function prototypes 
void *search(void *);
void print_it(void *);

// global variables
pthread_t  threads[NUM_THREADS];
pthread_mutex_t lock;
int tries;


/*
  main
//*/
int main()
{
  // create a number to search for
  pid_t pid = getpid();
  printf("the main thread's pid is: %d\n", (int) pid);

  // initialize the mutex lock 
  pthread_mutex_init(&lock, NULL); 
  printf("Searching for the number = %d...\n", (int) pid);
  printf("now %d threads will try to guess the number using srand()\n", NUM_THREADS);
  printf("they're guessing a number between %d and %d\n", 0, RAND_MAX);
  puts("this can take quite some time, watch on htop..");
  // create the searching threads
  int idx;
  for(idx = 0; idx < NUM_THREADS; ++idx){
    pthread_create(&threads[idx], NULL, search, (void*) pid);
  }
  
  // wait for (join) all the searching threads
  for (idx = 0; idx < NUM_THREADS; ++idx){ 
    pthread_join(threads[idx], NULL);
  }

  /*
    tries

    the tries variable will be set by the search threads in the meantime
    te main thread is waiting on the search threads (MAIN joined SEARCH)
  //*/

  printf("It took %d tries to find the number.\n", tries);
  
  // exit this thread */
  pthread_exit(0);

  // never executed
  puts("READY.");
  exit(EXIT_SUCCESS);
}


/*
  This is the cleanup function that is called when 
  the threads are cancelled 
*/
void print_it(void *arg)
{
  unsigned long *try = (unsigned long*) arg;
  pthread_t tid;
  
  // get the calling thread's ID
  tid = pthread_self();
  
  // print where the thread was in its search when it was cancelled
  printf("Thread %lu was canceled on its %lu try.\n", (unsigned long) tid, *try); 
}


/*
  This is the search routine that is executed in each thread 
*/
void* search(void* arg)
{
  int num = (int) arg;
  int idx = 0;
  pthread_t tid;
  
  // get the calling thread ID
  tid = pthread_self();
  
  // use the thread ID to set the seed for the random number generator
  srand(tid);
  
  /* 
     set the cancellation parameters --
     - Enable thread cancellation 
     - Defer the action of the cancellation 
  */
  
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  
  /* 
     push the cleanup routine (print_it) onto the thread
     cleanup stack.  This routine will be called when the 
     thread is cancelled.  Also note that the pthread_cleanup_push
     call must have a matching pthread_cleanup_pop call.  The
     push and pop calls MUST be at the same lexical level 
     within the code 
  */
  
  /* 
     pass address of 'idx' since the current value of 'idx' is not 
     the one we want to use in the cleanup function 
  */
  
  pthread_cleanup_push(print_it, (void*) &idx);
  
  // loop forever
  while (1) {
    ++idx;
    
    // does the random number match the target number?
    if (num == rand()) {
      
      /* 
	 try to lock the mutex lock --
	 if locked, check to see if the thread has been cancelled
	 if not locked then continue 
      */
      
      while(pthread_mutex_trylock(&lock) == EBUSY)
	pthread_testcancel();
      
      /* 
	 set the global variable for the number of tries 
      */
      
      tries = idx;
      
      printf("thread %lu found the number!\n", (unsigned long) tid);
      
      // cancel all the other threads 
      int cnt;
      for(cnt = 0; cnt < NUM_THREADS; ++cnt){
	if(threads[cnt] != tid){
	  printf("cancel #%d\n", cnt);
	  pthread_cancel(threads[cnt]);
	}
      }      

      // break out of the while loop
      break;
    }
    
    /* 
       every 100 tries check to see if the thread has been cancelled 
       if the thread has not been cancelled then yield the thread's
       LWP to another thread that may be able to run 
    */
    
    if (idx % 100 == 0) {
      pthread_testcancel();
      sched_yield();
    }
  }
  
  /* 
     The only way we can get here is when the thread breaks out
     of the while loop.  In this case the thread that makes it here
     has found the number we are looking for and does not need to run
     the thread cleanup function.  This is why the pthread_cleanup_pop
     function is called with a 0 argument; this will pop the cleanup
     function off the stack without executing it 
  */
  
  puts("clean up thread - pops");
  pthread_cleanup_pop(0);
  return((void *)0);
}
