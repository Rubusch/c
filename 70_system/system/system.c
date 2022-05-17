// system.c
/*
  usage:
  $ ./system.elf
      total 24
      -rw-r--r-- 1 pi pi  298 May 17 20:01 Makefile
      -rw-r--r-- 1 pi pi  873 May 17 19:58 system.c
      -rwxr-xr-x 1 pi pi 9136 May 17 20:00 system.elf
      -rw-r--r-- 1 pi pi 2524 May 17 20:00 system.o
      READY.


  Executing shell commands with system()

  #include <stdlib.h>
  int system(const char* command);
  // executes a console command

  Avoid using system() in set-user-ID and set-group-ID
  programs. Set-user-ID and set-group-ID programs should never use
  system() while operating under the program's privileged
  identifier. Even when such programs don't allow the user to specify
  the text of the command to be executed, the shel's reliance on
  various environment variables to control its operation means that
  the use of system() inevitably opens the door for a system security
  breach.

  system() calls and args might make the software non-portable.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main()
{
	system("ls -l");

	// won't be executed until system() is done
	printf("READY.\n");
	exit(EXIT_SUCCESS);
}
