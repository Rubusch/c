// sender.c
/*
    named pipe - half duplex connection, sender

    uses fifo, usage requires root privileges
//*/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../halfduplex.h"

#define ME "SCRATCHY:"
#define MESSAGE "Scratchymessage"


int main(int argc, char **argv)
{
  // open pipe
  fprintf(stderr, "%s started\n", ME);

  FILE *fp = NULL;
  fprintf(stderr, "%s connecting\n", ME);
  if (NULL == (fp = fopen(FIFO_PIPE, "w"))) {
    perror("fopen failed");
    exit(1);
  }

  fprintf(stderr, "%s sending \"%s\"\n", ME, MESSAGE);
  fputs(MESSAGE, fp);

  fclose(fp);
  fprintf(stderr, "%s done!\n", ME);
  exit(EXIT_SUCCESS);
}
