// execl.c
/*
  #include <unistd.h>
  int execl(const char* path, const char* arg, ..., (char*) 0);

  path
      the path to the program to execute
  arg
      a list of argumentes to the program to execute

  Executes a new programm from out of this program.
  The function returns an error code (0=Ok / -1=Fail)
//*/

#include <stdlib.h>

#include <unistd.h>


int main()
{
  execl("/bin/ls", "-r", "-t", "-l", (char *) 0);
  exit(0);
}
