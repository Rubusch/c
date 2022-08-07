/*
  virtual memory

  usage:
  From the last line of output, we can see that mprotect() has
  changed the permissions of the memory region to
  PROT_READ|PROT_WRITE.

  The string (deleted) that appears after the /dev/zero field is an
  artifact of the implementation of shared memory segments. Such
  segments are created as mapped files in an invisible tmpfs file
  system, and then later unlinked. Shared anonymous memory mappings
  are implemented in the same manner.
  (TLPI p. 1009, and TLPI chapter 49).

  $ ./virtual-memory.elf
    before mprotect()
    b6d08000-b6e08000 ---s 00000000 00:01 3078       /dev/zero (deleted)
    after mprotect()
    b6d08000-b6e08000 rw-s 00000000 00:01 3078       /dev/zero (deleted)
    READY.


  This program creates an anonymous mapping that initially has all
  access denied (PROT_NONE). The program then changes the protection
  on the region to read plus write. Before and after making the
  change, the program uses the system() function to exectue a shell
  command that displays the line from the /proc/PID/maps file
  corresponding to the mapped region, so that we can see the change in
  memory protection. (We could have obtained the mapping information
  by directly parsing /proc/self/maps, but we used the call to
  system() because it results in a shorter program.)


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1046
*/

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

#define LEN (1024 * 1024)
#define SHELL_FMT "cat /proc/%ld/maps | grep zero"
#define CMD_SIZE (sizeof(SHELL_FMT) + 20)  /* allow extra space for integer string */


int
main(int argc, char *argv[])
{
	char cmd[CMD_SIZE];
	char *addr;

	/*
	  create an anonymous mapping with all access denied
	*/

	addr = mmap(NULL, LEN, PROT_NONE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	/*
	  display line from /proc/self/maps corresponding to mapping

	  NB: snprintf() and vsnprintf() write at most size bytes
	  (including the terminating null byte ('\0')) to str.
	*/

	fprintf(stderr, "before mprotect()\n");
	snprintf(cmd, CMD_SIZE, SHELL_FMT, (long) getpid());
	system(cmd);

	/*
	  change protection on memory to allow read and write access
	*/

	if (-1 == mprotect(addr, LEN, PROT_READ|PROT_WRITE)) {
		perror("mprotect()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "after mprotect()\n");
	system(cmd);  // review protection via /proc/self/maps

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
