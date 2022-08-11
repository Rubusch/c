/*
  POSIX shared memory - read

  usage:

  TODO              


  The program displays the string in the existing shared memory object
  named in its command-line argument on standard output. After calling
  shm_open(), the program uses fstat() to determine the size of the
  shared memory and uses that size in the call to mmap() that maps the
  object and in the write() call that prints the string.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1113
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>


int
main(int argc, char *argv[])
{
	int fd;
	char *addr;
	struct stat sb;

	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <shm name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = shm_open(argv[1], O_RDONLY, 0);  // open existing object
	if (-1 == fd) {
		perror("shm_open()");
		exit(EXIT_FAILURE);
	}

	/*
	  use shared memory object size as length argument for mmap()
	  and as number of bytes to write()
	*/

	if (-1 == fstat(fd, &sb)) {
		perror("fstat()");
		exit(EXIT_FAILURE);
	}

	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	if (-1 == close(fd)) {  // fd is no longer needed
		perror("close()");
		exit(EXIT_FAILURE);
	}

	write(STDOUT_FILENO, addr, sb.st_size);
	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
