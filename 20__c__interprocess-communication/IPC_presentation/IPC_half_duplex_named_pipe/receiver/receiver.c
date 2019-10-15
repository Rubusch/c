// receiver.c
/*
  named pipe - half duplex connection, receiver

  usage as root!

  uses mkfifo() and enables the connection to processes without
  a fork (low level pipe)
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

#define ME "ITCHY:"


int main(int argc, char **argv)
{
  fprintf(stderr, "%s starting\n", ME);

  FILE *fp = NULL;
  char buf[BUF_SIZE];

  /*
    the file /tmp/fifo is created, the requested permissions are 0666,
    although they are affected by the umask setting as follows:
    final_umask = requested_permissions & ~( original_mask )

    thus it's necessary to clear the original_mask first
  //*/
  umask(0);
  mknod(FIFO_PIPE, S_IFIFO | 0666, 0);

  fprintf(stderr, "%s waiting..\n", ME);
  if (NULL == (fp = fopen(FIFO_PIPE, "r"))) {
    perror("fopen failed");
    exit(EXIT_FAILURE);
  }

  // polling
  while (1) {
    memset(buf, '\0', BUF_SIZE);

    if (NULL != fgets(buf, BUF_SIZE, fp)) {
      fprintf(stderr, "%s received \"%s\"\n", ME, buf);
      break;
    }
  }

  fclose(fp);
  fprintf(stderr, "%s done!\n", ME);
  exit(EXIT_SUCCESS);
}
