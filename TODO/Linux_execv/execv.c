// execv.c
/*
  #include <unistd.h>
  int execv(const char* path, char* const argv[]);

  path
      the path to the program to execute
  argv
      a list of argumentes to the program to execute

  Executes a new programm from out of this program.
  The function returns an error code (0=Ok / -1=Fail)

  Similar to execl but with an array.
//*/

#include <stdlib.h>

#include <unistd.h>


int main()
{
  char *flags[] = {"-r", "-t", "-l", ( char * )0};
  execv("/bin/ls", flags);
  exit(0);
}
