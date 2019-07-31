// overflow.c
/*
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void func(int a, int b, int c)
{
  char buf_1[5];
  char buf_2[10];
  int *ret;

  ret = buf_1 + 0x12; 
  (*ret) += 80;
}


int main()
{
  int x;
  x = 0;
  func(1,2,3);
  x = 1;
  printf("%d\n", x);
  
  return 0;
}
