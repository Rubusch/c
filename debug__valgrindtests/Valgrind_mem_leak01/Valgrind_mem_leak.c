// Valgrind_mem_leak.c
/*
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


int main(void)
{
  // init
  printf("init vars\n");
  int *a=NULL, *b=NULL;
  if( (a = (int*) malloc(sizeof(int))) == NULL) return EXIT_FAILURE;
  if( (b= (int*) malloc(sizeof(int))) == NULL) return EXIT_FAILURE;

  // assignment
  printf("*a = 5\n");
  *a = 5;

  printf("*b = 3\n");
  *b = 3;

  // Caution: no more pointer points to a!!
  printf("a = b\n");
  a = b;

  printf("free(a)\n");
  free(b);

  // Error! *a points to the same address as *b, the space allocated for *a isn't refered anymore.
  printf("free(b) - Oups!\n\n");
  free(a);

  return EXIT_SUCCESS;
}

/*
// Memory Leak

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_mem_leak
$ ./Valgrind_mem_leak.exe
init vars
*a = 5
*b = 3
a = b
free(a)
free(b) - Oups!

      7 [sig] Valgrind_mem_leak 988 f:\programming\TODO_Valgrind_mem_leak\Valgri
nd_mem_leak.exe: *** fatal error - called with threadlist_ix -1
Hangup

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_mem_leak
$ cat Valgrind_mem_leak.exe.stackdump
Stack trace:
Frame     Function  Args
0022C908  7C802532  (000007CC, 0000EA60, 000000A4, 0022C950)
0022CA28  6109773C  (00000000, 7C8025F0, 7C802532, 000000A4)
0022CB18  610952BB  (00000000, 003B0023, 00230000, 00000000)
0022CB78  6109579B  (0022CB90, 00000000, 00000094, 610A0EAA)
0022CC38  61095952  (000003DC, 00000006, 0022CC68, 61095B52)
0022CC48  6109597C  (00000006, 600301DC, 6113A581, 00402026)
0022CC68  61095B52  (0022D008, 611010E8, 0022CC90, 00660578)
0022CCE8  610C36F8  (00000001, 61169690, 00660090, 0022CC70)
0022CD98  61006198  (00000000, 0022CDD0, 61005510, 0022CDD0)
61005510  61004416  (0000009C, A02404C7, E8611001, FFFFFF48)
      7 [sig] Valgrind_mem_leak 988 f:\programming\TODO_Valgrind_mem_leak\Valgri
nd_mem_leak.exe: *** fatal error - called with threadlist_ix -1

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_mem_leak
$

//*/
