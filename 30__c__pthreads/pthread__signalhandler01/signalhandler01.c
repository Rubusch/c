// signalhandler01.c
/*
  shows a multithreaded signalhandler
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <pthread.h>
#include <signal.h>

#include <sys/types.h>
#include <unistd.h>


void *signal_handler(void *);


int main()
{
  sigset_t set;

  /*
     block all signals in main thread.  Any other threads that are
     created after this will also block all signals
  //*/
  sigfillset(&set);



  // set the signalmask to "set"
  if(0 != pthread_sigmask(SIG_SETMASK, &set, NULL)){
    perror("pthread sigmask failed");
    exit(EXIT_FAILURE);
  }

  /*
     create a signal handler thread.  This thread will catch all
     signals and decide what to do with them.  This will only
     catch nondirected signals.  (I.e., if a thread causes a SIGFPE
     then that thread will get that signal.
  //*/
  pthread_t thr;
  pthread_attr_t attr_thr;
  pthread_attr_init(&attr_thr);
  pthread_attr_setdetachstate(&attr_thr, PTHREAD_CREATE_DETACHED);


  // creating - no "THR_DAEMON" in POSIX!
  if(0 != pthread_create(&thr, &attr_thr, signal_handler, NULL)){
    perror("pthread create falied");
    exit(EXIT_FAILURE);
  }
  //  thr_create(NULL, 0, signal_hand, 0, THR_NEW_LWP|THR_DAEMON|THR_DETACHED, NULL); // TODO rm

  
  while (1) {
    /*
      Do your normal processing here....
    */
    puts("send SIGINT");
    puts("3");
    sleep(1);
    puts("2");
    sleep(1);
    puts("1");
    sleep(1);
    puts("send..");
    pthread_kill(thr, SIGINT);
    sleep(1); // to let the thread shutdown
  }

  pthread_attr_destroy(&attr_thr);
  exit(EXIT_SUCCESS);
}


/*
  signal handler as thread
//*/
void *signal_handler(void *arg)
{
  // for sigwait the signal needs to be blocked for other threads!!!!
  sigset_t waitset;
  sigemptyset( &waitset);
  sigaddset( &waitset, SIGINT);
  //  sigprocmask( SIG_BLOCK, SIGINT);

  int sig = (int) (intptr_t)arg;

  // catch all signals
  //  sigfillset(&set);

  // wait for a signal to arrive
  while (1) {

    switch (sigwait( &waitset, &sig)) {
    case 0:
      // here you would add whatever signal you needed to catch
      printf("Interrupted with signal %d, exiting...\n", sig);
      exit(EXIT_SUCCESS);

    default:
      printf("GOT A SIGNAL = %d\n", sig);
    }
  }

  //  return((void *)0);
  pthread_t thr = pthread_self();
  pthread_exit(&thr);
} 


