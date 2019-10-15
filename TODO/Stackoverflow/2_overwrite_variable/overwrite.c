// overwrite.c
/*
  this code overwrites the variable of the calling function
  by shifting the pointer of the subfunction

  the shift is in "byte", thus it's easier shift the pointer as char pointer
  (offset 1 byte) and pass it's address to an int pointer, since we want to
  overwrite an int (offset of int is 4 byte)
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHIFT 53

void func(int a, int b, int c)
{
  char buf_1[5];
  char *buf_2 = buf_1;

  buf_2 += SHIFT;
  int *tmp = ( int * )buf_2;
  *tmp = 7;
}


int main(void)
{
  int x = 0;
  printf("&x\t= %lu - x = %d\n", ( unsigned long )&x, x);
  func(1, 2, 3);
  printf("&x\t= %lu - x = %d\n", ( unsigned long )&x, x);
  exit(EXIT_SUCCESS);
}
