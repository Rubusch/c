// system.c
/*
  #include <stdlib.h>
  int system(const char* command);
      executes a console command - be aware of differences between the 'nix and win console
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main()
{
  system("ls -l");

  // won't be executed until system() is done
  puts("done");

  exit(0);
}
