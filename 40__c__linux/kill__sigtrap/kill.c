/*
  send SIGTRAP to pid passed as arg



  usage:
  $ ./rabbit.exe & ./foo.exe $(pidof rabbit.exe)
 */


#include <stdlib.h> /* atoi() */
#include <stdio.h> /* fprintf() */
#include <signal.h> /* kill() */

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: $0 <pid>\n");
    return 1;
  }
  kill(atoi(argv[1]), 3);

  return 0;
}
