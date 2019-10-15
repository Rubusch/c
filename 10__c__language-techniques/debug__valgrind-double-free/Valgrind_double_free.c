/* Valgrind_double_free.c
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
  Demo: causes a "Double Free" situation - to be checked with Valgrind/memcheck.
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT "foo"

int main(int argc, char **argv)
{
  // set up dyn var and init with text
  char *pText = NULL;
  if ((pText = ( char * )malloc((strlen(TEXT) + 1) * sizeof(char))) == NULL)
    return EXIT_FAILURE;
  strcpy(pText, TEXT);

  // show
  printf("My text is \"%s\"\n", pText);

  printf("First free()\n");
  free(pText);

  printf("Second free() - Ouch!!!\n\n");
  free(pText);

  return EXIT_SUCCESS;
}

/*
// Cygwin - Stackdump

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_double_free
$ ./Valgrind_double_free.exe
My text is "foo"
First free()
Second free() - Ouch!!!

      8 [sig] Valgrind_double_free 1832 f:\programming\TODO_Valgrind_double_free
\Valgrind_double_free.exe: *** fatal error - called with threadlist_ix -1
Hangup

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_double_free
$ cat Valgrind_double_free.exe.stackdump
Stack trace:
Frame     Function  Args
0022C908  7C802532  (000007CC, 0000EA60, 000000A4, 0022C950)
0022CA28  6109773C  (00000000, 7C8025F0, 7C802532, 000000A4)
0022CB18  610952BB  (00000000, 003B0023, 00230000, 00000000)
0022CB78  6109579B  (0022CB90, 00000000, 00000094, 610A0EAA)
0022CC38  61095952  (00000728, 00000006, 0022CC68, 61095B52)
0022CC48  6109597C  (00000006, 600301DC, 6113A581, 0040201E)
0022CC68  61095B52  (0022D008, 611010E8, 0022CC90, 610E4E78)
0022CCE8  610C36F8  (00000001, 61169690, 00660090, 0022CC70)
0022CD98  61006198  (00000000, 0022CDD0, 61005510, 0022CDD0)
61005510  61004416  (0000009C, A02404C7, E8611001, FFFFFF48)
      8 [sig] Valgrind_double_free 1832 f:\programming\TODO_Valgrind_double_free
\Valgrind_double_free.exe: *** fatal error - called with threadlist_ix -1

pt104496@pt1w194c /cygdrive/f/programming/TODO_Valgrind_double_free
$

//*/
