// timer.c
/*
  sets up a timer on a linux system, 
  the timer ticks down 250ms sec and prints out a message

  uses signals and sigaction()
//*/

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

void timer_handler(int signum)
{
  static int cnt = 0;
  printf("timer expired %d times\n", ++cnt);
}


int main()
{
  struct sigaction sa;
  struct itimerval timer;
  
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &timer_handler;
  sigaction(SIGVTALRM, &sa, NULL);

  // init 
  timer.it_value.tv_sec = 0;

  // set to 250ms expiry time
  timer.it_value.tv_usec = 250000;
  timer.it_interval.tv_sec = 0;

  // and every 250ms after that
  timer.it_interval.tv_usec = 250000;

  setitimer(ITIMER_VIRTUAL, &timer, NULL);

  // infinite!
  while(1);

  exit(EXIT_SUCCESS);
}
