// static.c
/*
  demonstrates a static lib
//*/

#include <stdlib.h>
#include <stdio.h>

void test1(int*);
void test2(int*);

int main()
{
  int val=7;
  printf("val (before call)\t = %d\n", val);
  test1(&val);
  printf("val (after call)\t = %d\n", val);

  exit(EXIT_SUCCESS);
}

