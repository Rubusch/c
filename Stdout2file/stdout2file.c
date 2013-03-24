// stdout2file.c
/*
  freopen() - connect the output stream to the filestream
//*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  char* filename = "foobar.txt";
  printf("We open the file %s\n", file);
  freopen(filename, "a+", stdout);
  printf("All work and no play makes Jack a dull boy!\n");

  return EXIT_SUCCESS;
};
