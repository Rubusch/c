/* Valgrind_mem_leak.c
 *
 * execute
 * $ valgrind *.exe
 * or
 * $ valgrind -v *.exe
 *
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

/*
  Demo: causes a "Memory Leak" situation - to be checked with Valgrind/memcheck.
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int func()
{
  char* pText = NULL;
  if( (pText = (char*) malloc(12345 * sizeof(char))) == NULL) return -1;
  return 0;
}


int main(void)
{
  // init
  printf("init a variable and hang up..\n");
  int cnt;
  for(cnt = 0; cnt < 10; ++cnt) func();
  return EXIT_SUCCESS;
}

/*
// a lot of untouchable memory was reserved, using: while(1)

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_mem_leak02
$ ./Valgrind_mem_leak.exe
init a variable and hang up..

//*/
