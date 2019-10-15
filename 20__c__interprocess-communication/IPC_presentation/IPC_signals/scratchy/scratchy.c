// scratchy.c
/*
  signals
//*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lotharlib/console_input.h"

#define ME "SCRATCHY:"
#define PID_DIGITS 4


int main(int argc, char **argv)
{
  // todo send signal
  pid_t pid = 0;
  readnumber(( unsigned int * )&pid, PID_DIGITS,
             "enter a valid pid (up to 4 digits) to connect to");

  unsigned int chosen = 0;
  do {
    fprintf(stderr,
            "\n%s enter an action:\n\t(0) to quit,\n\t(1) to send "
            "SIGALRM,\n\t(2) to send SIGKILL to %d\n",
            ME, pid);
    readdigit(&chosen, "\tselect");
    switch (chosen) {
    case 0:
      break;

    case 1:
      fprintf(stderr, "%s sending SIGALRM\n", ME);
      kill(pid, SIGALRM);
      break;

    case 2:
      fprintf(stderr, "%s sending SIGKILL\n", ME);
      kill(pid, SIGKILL);
      chosen = 0;
      break;

    default:
      fprintf(stderr, "%s didn't understand..\n", ME);
      break;
    }
  } while (chosen);

  fprintf(stderr, "%s terminating\n", ME);
  fprintf(stderr, "%s done!\n", ME);
  exit(EXIT_SUCCESS);
}
