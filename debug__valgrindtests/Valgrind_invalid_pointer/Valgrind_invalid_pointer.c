// Valgrind_invalid_pointer.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

/*
  Demo: causes a "Invalid Pointer" situation - to be checked with Valgrind/memcheck.
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv)
{
  // init
  printf("init pointer to 10 char's\n");
  char* pText = NULL;
  if( (pText = malloc(10 * sizeof(char))) == NULL) return EXIT_FAILURE;
  memset(pText, '6', 10);

  printf("assign \'7\' to pText, index \"10\" - Dangerous, \nonly 10 elements allocated, last index is 9!\n\n");
  pText[10] = '7'; // Jackpot!!!

  return EXIT_SUCCESS;
}

/*
// Valgrind is able to find this (above), but it's not able to find the same using static memory, e.g.:

int main()
{
    char text[10];
    text[11] = 'X';
}



//*/
