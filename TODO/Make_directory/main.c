// main.c

#include <stdlib.h>
#include <stdio.h>

#include "directory/folder.h"


extern void func(const char*);

int main(int argc, char** argv)
{
  func("Hello World!");

  exit(EXIT_SUCCESS);
}
