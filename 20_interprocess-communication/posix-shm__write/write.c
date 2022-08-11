/*
  POSIX shared memory - write

  usage:

  TODO              


  The listing demonstrates the use of a shared memory object to
  transfer data from one process to another. the program copies the
  string contained in its second command-line argument into the
  existing shared memory object named in its first command-line
  argument. Before mapping the object and performing the copy, the
  program uses ftruncate() to resize the shared memory obejct to be
  the same length as the string that is to be copied.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1112
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
	int fd;
	size_t size;  // size of the shared memory
	void *addr;

	if (3 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <shm name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = shm_open(argv[1], O_RDWR, 0);  // open existing object
	if (-1 == fd) {
		perror("shm_open()");
		exit(EXIT_FAILURE);
	}

	size = strlen(argv[2]);
	if (-1 == ftruncate(fd, size)) {  // resize shm object to hold string
		perror("ftruncate()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "resized to %ld bytes\n", (long) size);

	addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	if (-1 == close(fd)) {
		perror("close()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "copying %ld bytes\n", (long) size);
	memcpy(addr, argv[2], size);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
