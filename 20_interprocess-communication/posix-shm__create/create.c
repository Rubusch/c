/*
  POSIX shared memory - create

  usage:

  Shows how to use the program to create a 10'000 byte shared memory
  object, and then use ls to show this object in /dev/shm

  TODO              


  The listing provides a simple example of the use of shm_open(),
  ftruncate(), and mmap(). This program creates a shared memory object
  whose size is specified by a command-line argument, and maps the
  object into th eprocess's virtual address space. (The mapping step
  is redundant, since we don't actually do anything with the shared
  memory, but it serves to demonstrate the use of mmap().) The program
  permits the use of command-line options to select flags (O_CREAT and
  O_EXCL) for the shm_open() call.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1110
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>

#include "tlpi/get_num.h"


static void
usage(const char *name)
{
	fprintf(stderr, "usage:\n"
		"$ %s [-cx] <name> <size> [octal-permissions]\n",
		name);
	fprintf(stderr, "\t-c\tcreate shared memory (O_CREAT)\n");
	fprintf(stderr, "\t-x\tcreate exclusively (O_EXCL)\n");

	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int flags, opt, fd;
	mode_t perms;
	size_t size;
	void *addr;

	flags = O_RDWR;
	while (-1 != (opt = getopt(argc, argv, "cx"))) {
		switch (opt) {
		case 'c':
			flags |= O_CREAT;
			break;
		case 'x':
			flags |= O_EXCL;
			break;
		default:
			usage(argv[0]);
		}
	}

	if (argc <= optind + 1) {
		usage(argv[0]);
	}

	size = get_long(argv[optind + 1], GN_ANY_BASE, "size");
	perms = (argc <= optind + 2)
		? (S_IRUSR | S_IWUSR)
		: get_long(argv[optind + 2], GN_BASE_8, "octal-perms");

	/*
	  create shared memory object and set its size
	*/

	// shm_open()
	fd = shm_open(argv[optind], flags, perms);
	if (-1 == fd) {
		perror("shm_open()");
		exit(EXIT_FAILURE);
	}

	// ftruncate()
	if (-1 == ftruncate(fd, size)) {
		perror("ftrunctate()");
		exit(EXIT_FAILURE);
	}

	/*
	  map shared memory object
	*/

	// mmap()
	addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
