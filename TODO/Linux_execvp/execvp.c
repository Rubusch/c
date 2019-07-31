// execvp.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-September-27



  #include <unistd.h>
  int execvp(const char* path, char* const argv[]);

  path
      the path to the program to execute
  argv
      a list of argumentes to the program to execute

  Executes a new programm from out of this program. The function returns an
  error code (0=Ok / -1=Fail)

  execvp() uses the environmental variable PATH to resolve the name of a prg,
  hence it doesn't need the full path, if the prg is listed.

  Similar to execlp but with an array.
//*/

#include <stdlib.h>

#include <unistd.h>


int main()
{
  char* flags[] = {"-r", "-t", "-l", (char*) 0};
  execvp("ls", flags);
  exit(0);
}
