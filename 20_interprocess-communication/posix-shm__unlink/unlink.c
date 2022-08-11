/*
  POSIX shared memory - unlink

  usage:

  TODO              


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
