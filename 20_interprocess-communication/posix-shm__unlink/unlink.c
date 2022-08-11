/*
  POSIX shared memory - unlink

  usage:
  $ ./create.elf -c /demo_shm 10000
    READY.

  $ ls -l /dev/shm/
    total 0
    -rw------- 1 pi pi 10000 Aug 11 23:22 demo_shm

  $ ./write.elf /demo_shm 'hello'
    resized to 5 bytes
    copying 5 bytes
    READY.

  $ ls -l /dev/shm
    total 4
    -rw------- 1 pi pi 5 Aug 11 23:24 demo_shm

  $ ./read.elf /demo_shm
    hello
    READY.

  $ ./unlink.elf /demo_shm
    READY.


  The program uses shm_unlink() to remove the shared memory obejct
  specified in the program's command-line argument.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1114
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>


int
main(int argc, char *argv[])
{
	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <shm name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (-1 == shm_unlink(argv[1])) {
		perror("shm_unlink()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
