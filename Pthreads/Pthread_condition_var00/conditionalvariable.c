// conditionalvariable.c
/*
  demonstrates the use of a conditional variable
  1. conditional variables must be used with mutexes!
  2. a conditional variable is similar to the thr_suspend() function under Solaris UNIX

  variables can be declared globally or locally, depending on the situation, this example 
  here demonstrates some techniques to do both
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>


// conditional variables - replace the THR_SUSPEND under Solaris
pthread_cond_t cv_suspend;
pthread_mutex_t mx_suspend;

// thread identifier o the thread to be suspended
pthread_t thr_sleepy;

// a thread function
void* thread_sleepy(void*);


int main()
{
  /*
    1. init the conditional variable's attribute (when using it globally, default is just the local space - NULL)
  //*/

  // cv: first init the cv attribute
  pthread_condattr_t attr_cv_suspend; // here local variable
  pthread_condattr_init(&attr_cv_suspend);

  // cv: set global using PTHREAD_PROCESS_SHARED, or just local scope with PTHREAD_PROCESS_PRIVATE
  pthread_condattr_setpshared(&attr_cv_suspend, PTHREAD_PROCESS_SHARED); 

  // cv: init the cv, attr_cv set to NULL would result in PTHREAD_PROCESS_PRIVATE (default)
  pthread_cond_init(&cv_suspend, &attr_cv_suspend);



  /*
    2. init the mutex attribute (when using it globally, default is just the local space - NULL)
  //*/

  pthread_mutexattr_t attr_mx_suspend; // here local variable
  pthread_mutexattr_init(&attr_mx_suspend);

  // cv-mx: set global using PTHREAD_PROCESS_SHARED
  pthread_mutexattr_setpshared(&attr_mx_suspend, PTHREAD_PROCESS_SHARED);

  // cv-mx: init the mutex
  pthread_mutex_init(&mx_suspend, &attr_mx_suspend);



  /******* do the thread **********/

  if(0 != pthread_create(&thr_sleepy, NULL, thread_sleepy, NULL)){
    perror("MAIN: pthread_create failed");
    exit(EXIT_FAILURE);
  }
  puts("MAIN: thread created!");
  


  /*
    3. suspend thread
  //*/
  //  puts("MAIN: now suspending the thread...");
  // TODO

  // wait - this simulates some action in this thread..
  sleep(1);


  /*
    4. awake suspended thread
  //*/
  puts("MAIN: now resuming the suspended thread, again");
  // cv: awake conditional variable
  pthread_cond_signal(&cv_suspend);

  /*
    cleaning up - necessary or we will have a memory leak
  //*/

  // wait to let thread catch up
  sleep(1); 

  // cv: destroy the conditional variable stuff
  puts("MAIN: cleaning up conditional variable stuff");
  pthread_cond_destroy(&cv_suspend);
  pthread_mutex_destroy(&mx_suspend);
  pthread_mutexattr_destroy(&attr_mx_suspend);
  pthread_condattr_destroy(&attr_cv_suspend);

  puts("READY.");
  exit(EXIT_SUCCESS);
}



void* thread_sleepy(void* arg)
{
  puts("THREAD: this is the dummy thread");
  
  // thread sleepy suspends
  puts("THREAD: suspends...");

  /*
    cv: suspend thread B using a conditional variable
  //*/
  pthread_mutex_lock(&mx_suspend);
  pthread_cond_wait(&cv_suspend, &mx_suspend);
  pthread_mutex_unlock(&mx_suspend);

  puts("THREAD: ...awakes");
  puts("THREAD: exits");
  pthread_exit(0);
}
