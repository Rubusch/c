// func123.c
/*
  this programm results in a segmentation violation
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void func(char *str)
{
  char buffer[16];
  strcpy(buffer, str);
}


int main(void)
{
  char large_str[256];
  int idx;
  for (idx = 0; idx < 256; ++idx) {
    large_str[idx] = 'A';
  }

  func(large_str);
}
