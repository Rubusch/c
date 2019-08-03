// signalhandler02.c
/*
 * Rich Schiavi writes:  		Sept 11, 1994
 *
 * I believe the recommended way to kill certain threads is
 * using a signal handler which then will exit that particular
 * thread properly. I'm not sure the exact reason (I can't remember), but
 * if you take out the signal_handler routine in my example, you will see what
 * you describe, as the main process dies even if you send the
 * thr_kill to the specific thread.
 *
 * Dave Marshall writes:
 * I whipped up a real quick simple example which shows this using
 * some sleep()s to get a good simulation.
 *
 *
 * And Lothar writes:
 * ...translated into Posix :-P
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <signal.h>
#include <unistd.h>


static 	void* first_thread();
static 	void* second_thread();
void ExitHandler(int);

static pthread_t thr_one, thr_two, thr_main;

// better: hanlde this states with conditional variables & mutexes!
int first_active = 1 ; 
int second_active = 1;


int main()
{
  struct sigaction act;  
  act.sa_handler = ExitHandler;
  sigemptyset(&act.sa_mask);
  sigaction(SIGTERM, &act, NULL); 
  
  // get own tid
  thr_main = pthread_self();
  
  // create threads thr_one and thr_two
  if(0 != pthread_create(&thr_one, NULL, first_thread, NULL)){
    perror("pthread create failed");
    exit(EXIT_FAILURE);
  }
  if(0 != pthread_create(&thr_two, NULL, second_thread, NULL)){
    perror("pthread create failed");
    exit(EXIT_FAILURE);
  }
  
  int idx = 0;
  for(idx = 0; idx < 10; idx++){
    fprintf(stderr, "main loop: %d\n", idx);
    if(idx == 5)	{
      pthread_kill(thr_one, SIGTERM);
    }
    sleep(3);
  }
  pthread_kill(thr_two, SIGTERM);
  sleep(5);
  fprintf(stderr, "main exit\n");
  
  puts("READY.");
  exit(EXIT_SUCCESS);
}


/*
  thread 1
//*/
static void *first_thread()
{
  pthread_t thr = pthread_self();
  
  fprintf(stderr, "first_thread id: %lu\n", (unsigned long) thr);

  int idx = 0;
  while (first_active){
    fprintf(stderr, "first_thread: %d\n", idx++);
    sleep(2);
  }
  fprintf(stderr, "first_thread exit\n");
  
  pthread_exit(&thr);
}


/*
  thread 2
//*/
static void *second_thread()
{
  int idx = 0;
  
  fprintf(stderr, "second_thread id: %lu\n", (unsigned long) pthread_self());
  
  while(second_active){
    fprintf(stderr, "second_thread: %d\n", idx++);
    sleep(3);
  }
  fprintf(stderr, "second_thread exit\n");
  
  pthread_t thr = pthread_self();
  pthread_exit(&thr);
}


/*
  thread 3
//*/
void ExitHandler(int sig)
{
  pthread_t thr = pthread_self();  
  fprintf(stderr, "ExitHandler thread id: %lu\n", (unsigned long) thr);

  pthread_exit(&thr);  
}
