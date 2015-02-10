// daemon.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-September-27



  #include <unistd.h>
  int chdir(const char* path);

  Changes working directory for the process, returns 0 if ok, else -1.



  #include <sys/types.h>
  #include <unistd.h>
  pid_t getpid();

  Returns the current pid of the porcess. getppid() returns the pid of
  the parent process.



  #include <unistd.h>
  pid_t setsid();

  Returns the session ID of the calling process, or -1.



  #include <unistd.h>
  int dup(int oldfd);
  int dup2(int oldfd, newfd);

  creates and returns a copy of the file descriptor "oldfd", or -1.
  dup2 makes "newfd" a copy of "oldfd", closing "newfd" first if necessary.



  #include <unistd.h>
  unsigned sleep(unsigned seconds);

  Shall cause the calling thread to be suspended from execution untill either
  "seconds" of realtime seconds has elapsed or a signal (e.g. SIGALARM) is
  delivered to the calling thread.

  Pitfall: unspecified interactions between sleep() and setitimer(), ualarm(),
  usleep()
  Returns 0 if ok, else "unslept"amount of seconds, if awoken by a signal.
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>


void start_daemon()
{
  chdir("/"); // senseless here, but ok
  setsid();

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  open("/dev/null", O_RDWR);
  dup(STDIN_FILENO);
  dup(STDIN_FILENO);

  while(1) sleep(100);
}


int main(int argc, char** argv)
{
  pid_t pidChild=0;

  if(0 > (pidChild = fork())){
    perror("fork() failed");
    return EXIT_FAILURE;
  }

  if(pidChild == 0) start_daemon();
  printf("child has PID %i\r\n", pidChild);

  pid_t pidParent=getpid();
  printf("Parent has (had) PID %i\r\n", pidParent);

  return EXIT_SUCCESS;
}

