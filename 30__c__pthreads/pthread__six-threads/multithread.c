// multithread.c
/*
  multithread - solaris, transpated to POSIX

  pthread_join()
      1. arg - thread to join
      2. arg - return value pointed to, when thread terminates correctly


  Solaris: thr_join()
     1.arg - suspend calling thread until 1.arg thread terminates
     2.arg - points to the id of the 1.arg thread if the join terminates
successfully 3.arg - status argument


  global pthread_t variables are named "thr_"
  local pthread_t variables are named "thread_"
  global pthread_cond_t is "cv_" and is used to demonstrate "suspend" of a
thread
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h> // pthread_create(), pthread_join(), ...
#include <unistd.h>  // sleep()


void *sub_a(void *);
void *sub_b(void *);
void *sub_c(void *);
void *sub_d(void *);
void *sub_e(void *);
void *sub_f(void *);


pthread_t thr_a, thr_b, thr_c;


// conditional variables - replace the THR_SUSPEND under Solaris
pthread_cond_t cv_suspend;
pthread_mutex_t mx_suspend;


int main()
{
  // new code:
  puts("multithread");

  /*
    init the conditional variable with the mutex
  //*/
  // cv: first init the cv attribute
  pthread_condattr_t attr_cv_suspend;
  pthread_condattr_init(&attr_cv_suspend);
  // cv: set global using PTHREAD_PROCESS_SHARED, or just local scope with
  // PTHREAD_PROCESS_PRIVATE
  pthread_condattr_setpshared(&attr_cv_suspend, PTHREAD_PROCESS_SHARED);
  // cv: init the cv, attr_cv set to NULL would result in
  // PTHREAD_PROCESS_PRIVATE (default)
  pthread_cond_init(&cv_suspend, &attr_cv_suspend);

  pthread_mutexattr_t attr_mx_suspend;
  pthread_mutexattr_init(&attr_mx_suspend);
  // cv-mx: set global using PTHREAD_PROCESS_SHARED
  pthread_mutexattr_setpshared(&attr_mx_suspend, PTHREAD_PROCESS_SHARED);
  // cv-mx: init the mutex
  pthread_mutex_init(&mx_suspend, &attr_mx_suspend);


  // create main thread
  pthread_t main_thr = pthread_self();

  printf("MAIN: Main thread = %lu\n", ( unsigned long )main_thr);
  do {
    if (0 != pthread_create(&thr_b, NULL, sub_b, NULL)) {
      perror("MAIN: pthread_create failed");
      break;
    }
    printf("MAIN: B created: %lu\n", ( unsigned long )thr_b);

    if (0 != pthread_create(&thr_a, NULL, sub_a, ( void * )&thr_b)) {
      perror("MAIN: pthread_create failed");
      break;
    }
    printf("MAIN: A created: %lu\n", ( unsigned long )thr_a);

    if (0 != pthread_create(&thr_c, NULL, sub_c, &main_thr)) {
      perror("MAIN: pthread_create failed");
      break;
    }
    printf("MAIN: C created: %lu\n", ( unsigned long )thr_c);
  } while (0); // improves cleaning up and debugging!

  printf("MAIN: Main Created threads A:%lu B:%lu C:%lu\n",
         ( unsigned long )thr_a, ( unsigned long )thr_b,
         ( unsigned long )thr_c);

  puts("MAIN: busy for 3 secs");
  sleep(3);

  /*
    B doesn't need the conditional variable at this state
    anymore (due to a correct utilization of sleep's... well),
    so we're cleaning up!
  //*/

  // cv: destroy the conditional variable stuff
  puts("MAIN: cleaning up conditional variable of B");
  pthread_cond_destroy(&cv_suspend);
  pthread_mutex_destroy(&mx_suspend);
  pthread_mutexattr_destroy(&attr_mx_suspend);
  pthread_condattr_destroy(&attr_cv_suspend);

  puts("MAIN: cleaning up done!");
  puts("MAIN: Main Thread exits!\n");
  pthread_exit(0);
  exit(EXIT_SUCCESS);
}


void *sub_a(void *arg)
{
  puts("A: In thread A:");

  // receives thr_b as pointer (suspended)...
  pthread_t *thread_b = ( pthread_t * )arg;
  pthread_t thread_d;
  printf("A: Received thread B %lu\n", ( unsigned long )*thread_b);

  // ...and passes thr_b to the freshly created thread_d (via local variables)
  if (0 != pthread_create(&thread_d, NULL, sub_d, thread_b)) {
    perror("A: pthread create failed");
    return NULL;
  }
  printf("A: Created thread D:%lu\n", ( unsigned long )thread_d);

  puts("A: busy for 1 sec");
  sleep(1);

  puts("A: Thread exits\n");
  pthread_exit(0);
}


void *sub_b(void *arg)
{
  printf("B: In thread B: %lu\n", ( unsigned long )pthread_self());

  // B suspends
  puts("B: suspends...");
  // cv: suspend thread B using a conditional variable
  pthread_mutex_lock(&mx_suspend);
  pthread_cond_wait(&cv_suspend, &mx_suspend);
  pthread_mutex_unlock(&mx_suspend);
  puts("B: ...awakes");

  puts("B: busy for 5 secs");
  sleep(5);


  puts("B: Thread exits!\n");
  pthread_exit(0);
}


void *sub_c(void *arg)
{
  puts("C: In thread C:");

  puts("C: received thread MAIN");
  pthread_t thread_main = ( pthread_t )arg;

  // create F
  puts("C: creates thread F now (should be as a daemon in Solaris), which is "
       "here basically a local thread ID");
  pthread_t thread_f;
  if (0 != pthread_create(&thread_f, NULL, sub_f, NULL)) {
    perror("C: pthread_create failed");
    return NULL;
  }

  // join MAIN
  puts("C: Will wait on MAIN thread (joins MAIN)");
  int *return_status = NULL;
  if (0 != pthread_join(thread_main, ( void * )&return_status)) {
    perror("C: pthread_join failed");
    return NULL;
  }
  printf("C: ...MAIN thread (%lu) returned thread (%d)\n",
         ( unsigned long )thread_main, *return_status);

  puts("C: busy for 1 secs");
  sleep(1);

  puts("C: Thread exits!\n");
  pthread_exit(0);
}


void *sub_d(void *arg)
{
  printf("D: In thread D:\n");

  // B was received (still suspended) as parameter
  puts("D: received thread B");
  pthread_t *thread_b = ( pthread_t * )arg;

  puts("D: creates thread E");
  pthread_t thread_e;
  if (pthread_create(&thread_e, NULL, sub_e, NULL)) {
    perror("D: pthread_create failed");
    return NULL;
  }
  printf("D: Created thread E:%lu\n", ( unsigned long )thread_e);

  // awakes B
  printf("D: D awakes B thread = %lu\n", ( unsigned long )*thread_b);
  // cv: awake conditional variable
  pthread_mutex_lock(&mx_suspend);
  pthread_cond_signal(&cv_suspend);
  pthread_mutex_unlock(&mx_suspend);

  // joins E
  puts("D: Will wait on thread E (D joins E)...");
  int return_status;
  if (0 != pthread_join(thread_e, ( void * )&return_status)) {
    perror("D: pthread_join failed");
    return NULL;
  }
  printf("D: ...E thread (%lu) returned thread (%d)\n",
         ( unsigned long )thread_e, return_status);

  puts("D: busy for 1 sec");
  sleep(1);

  puts("D: Thread D exits!\n");
  puts("D: the proggy ends here!\n\n\n\nREADY."); // end of program!
  pthread_exit(0);
}


void *sub_e(void *arg)
{
  puts("E: In thread E:");

  // join 1
  int return_status;
  puts("E: 1. Will wait on thread A (E joins A)...");
  if (0 != pthread_join(thr_a, ( void * )&return_status)) {
    perror("E: pthread_join failed");
    return NULL;
  }
  printf("E: ...A thread returned %d\n", ( int )return_status);

  // join 2
  puts("E: 2. Will wait another time on B (E joins B)...");
  if (0 != pthread_join(thr_b, ( void * )&return_status)) {
    perror("E: pthread_join failed");
    return NULL;
  }
  printf("E: ...B thread (%lu) returned thread (%d)\n", ( unsigned long )thr_b,
         ( int )return_status);

  // join 3
  puts("E: 3. Will wait on thread C again (E joins C)...");
  if (0 != pthread_join(thr_c, ( void * )&return_status)) {
    perror("E: pthread_join failed");
    return NULL;
  }
  printf("E: ...C thread (%lu) returned thread (%d)\n", ( unsigned long )thr_c,
         ( int )return_status);

  // do some counting
  puts("E: busy for 1 sec");
  sleep(1);

  puts("E: Thread exits\n");
  pthread_exit(0);
}


void *sub_f(void *arg)
{
  printf("F: In thread F...\n");


  puts("F: Goes \"sleep\"-ing");
  sleep(1);

  puts("F: Does some counting");
  int idx = 0;

  for (idx = 0; idx < 10; idx++)
    printf("F: Thread F is still running - %d/10\n", (idx + 1));

  puts("F: Thread exits!\n");
  pthread_exit(0);
}
