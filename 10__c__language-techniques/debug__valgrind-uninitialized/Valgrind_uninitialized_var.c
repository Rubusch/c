/* Valgrind_uninitialized_var.c
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
  Demo: causes a "Uninitialized Variable" situation - to be checked with Valgrind/memcheck.
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void func(int iVar)
{
  printf("if(0 == %i)\n", iVar);
  if(0 == iVar) printf("\tTRUE - Just by chance!\n");
  else printf("\tFALSE - By chance not!\n");
  printf("\n");

  return;
}

int main(int argc, char** argv)
{
  printf("define but no init..\n");
  unsigned int iVar; // allways initialize variables
  // A "gcc -Wall" already warns from this, "-Werror" will fail to build

  int cnt;
  for(cnt = 0; cnt < 10; ++cnt) func(iVar);
  return EXIT_SUCCESS;
}


/*
// comparing a uninitialized variable with something works only by chance, gcc warns about this already!

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_uninitialized_var
$ make clean && make
rm -f Valgrind_uninitialized_var.exe *~ *.o core
gcc -c -O -g -Wall -std=c99 Valgrind_uninitialized_var.c
Valgrind_uninitialized_var.c: In function `main':
Valgrind_uninitialized_var.c:24: warning: 'iVar' might be used uninitialized in
this function
gcc -o Valgrind_uninitialized_var Valgrind_uninitialized_var.o -lm

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_uninitialized_var
$ ./Valgrind_uninitialized_var.exe
define but no init..
if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!

if(0 == 1628438944)
        FALSE - By chance not!
//*/
