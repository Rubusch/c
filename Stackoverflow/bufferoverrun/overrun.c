// overrun.c
/*
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
  puts("overrun test");

  /* compiles without problems:
  // although buf only has 10 elements!
  char input[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  char buf[10];
  strcpy(buf, input);
  //*/ 

  //* compiles without problems, too!
  // already on purpose overrunning the limit of 10 elements!
  char input[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  char buffer[10];
  strncpy(buffer, input, sizeof(buffer));
  //*/

  puts("READY.");
  exit(EXIT_SUCCESS);
} 
