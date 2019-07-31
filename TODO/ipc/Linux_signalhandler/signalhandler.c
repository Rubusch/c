// signalhanlder
/*
  demonstration of a signal handler

  experimental grounds to play with using e.g. htop and sending signals 
  to the application, e.g. SIGNINT - depending on the implementation/modifications
  the signal will terminate the program or the program will continue
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

int flag = 0;

void sighandler()
{
  signal(SIGINT, sighandler);
  flag = 0;
}


int main(int argc, char** argv)
{
  // ...
  signal(SIGINT, sighandler);
  // ...
  flag = 1;
  while(flag)
    pause();
  // ...
  exit(EXIT_SUCCESS);
}

