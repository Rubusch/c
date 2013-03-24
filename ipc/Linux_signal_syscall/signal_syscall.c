// signal_syscall.c
/*
  demonstrates the reaction on a sent signal
  Send a SIGKILL (e.g. with htop) to end the prg!
//*/

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void catch_int(int sig)
{
  signal(SIGINT, catch_int);
  //  puts("Eh eh that tickles...\n");
  printf("Eh eh that tickles...\n");
  fflush(stdout);
}


int main(int argc, char** argv)
{
  // set the INT (CTRL+C) signal handler to catch_int
  signal(SIGINT, catch_int);
  
  // infinite loop
  while(1) pause();
}
