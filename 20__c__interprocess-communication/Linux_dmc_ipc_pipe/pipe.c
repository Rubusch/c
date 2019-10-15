// pipe.c
/*
  pipe() - low level pipes
  int pipe(int fd[2])
  fd[0]    reading
  fd[1]    writing
   - communication: read(), write()
   - to be closed with close(int fd)

   Example from the man pages
//*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char **argv)
{
  int pfd[2];
  pid_t cpid;
  char buf;

  assert(argc == 2);
  /*
    ./pipe foo
  //*/

  // pipe
  if (0 > pipe(pfd)) {
    perror("pipe failure");
    exit(EXIT_FAILURE);
  }

  // fork
  if (0 > (cpid = fork())) {
    perror("fork failed");
    return EXIT_FAILURE;

  } else if (0 == cpid) {
    /* child code - read from pipe */

    // close unused write - idx == 1 // write
    close(pfd[1]);

    // read out of pipe - idx == 0 // read
    while (0 < read(pfd[0], &buf, 1)) {
      write(STDOUT_FILENO, &buf, 1);
    }
    write(STDOUT_FILENO, "\n", 1);

    // and close
    close(pfd[0]);

    _exit(EXIT_SUCCESS);

  } else {
    /* parent code */

    // close unused read - idx == 0 // read
    close(pfd[0]);

    // write into pipe - idx == 1 // write
    write(pfd[1], argv[1], strlen(argv[1]));

    // reader will see EOF
    close(pfd[1]);

    // wait for child
    wait(NULL);

    exit(EXIT_SUCCESS);
  }
}
