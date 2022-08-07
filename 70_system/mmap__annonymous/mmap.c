/*
  mmap demo

  usage:
  $ ./mmap.elf
    child: value = 1
    parent: value = 2
    READY.


  mmap()

  The mmap() system call creates a new memory mapping in the calling
  process's virtual addrss space. A mapping can be of two types:

  - File mapping: A file mapping maps a region of a file directly into
    the calling process's virtual memory (file-based mapping or
    memory-based mapping).

  - Anonymous mapping: An anonymous mapping doesn't have a
    corresponding file. Instead, the pages of the mapping are
    initialized to 0.

  When two or more processes share the same pages, each process can
  potentially see the changes to the page contents made by other
  processes, depending whether the mapping is private or shared.

  - Private mapping (MAP_PRIVATE): Modifications to the contents of
    the mapping are not visible to other processes and, for a file
    mapping, are not carried through to the underlying file. A
    MAP_PRIVATE mapping is sometimes referred to as a private,
    copy-on-write mapping.

  - Shared mapping (MAP_SHARED): Modifications to the contents of the
    mapping are visible to other processes that share the same mapping
    and, for a file mapping, are carried through to the underlying
    file.

  Purpose of various types of memory mappings

   Visibility of |                     Mapping type
   modifications |       File              |         Anonymous
  ---------------+-------------------------+--------------------------
   Private       | Initializing memory     | Memory allocation
                 | from contents of file   | exmple: malloc()
                 | example: copy-on-write  |
  ---------------+-------------------------+--------------------------
   Shared        | Memory-mapped I/O;      | Shared memory between
                 | sharing memory          | processes (IPC)
                 | between processes (IPC) |
                 |                         |


  The program demonstrates the use of either MAP_ANONYMOUS or
  /dev/zero to share a mapped region between parent and hcild
  processes. The choice of technique is determined by whether
  USE_MAP_ANON is defined when compiling the program. The parent
  initializes an integer in the shared region to 1 prior to calling
  fork(). The child then increments the shared integer and exits,
  while the parent waits for the child to exit and then prints the
  value of the integer.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1036
*/

#ifdef USE_MAP_ANON
#define _DEFAULT_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
	int *addr;  // pointer to shared memory region

#ifdef USE_MAP_ANON  /* use MAP_ANONYMOUS */
        /*     mmap(addr, lenth      , prot                , flags                   , fd, offset);  */
	addr = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

#else  /* map /dev/zero */
	int fd;

	fd = open("/dev/zero", O_RDWR);
	if (-1 == fd) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	addr = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	if (-1 == close(fd)) {  // fd is no longer needed
		perror("close()");
		exit(EXIT_FAILURE);
	}
#endif

	*addr = 1;  // initialize integer in mapped region

	switch (fork()) {  // parent and child share mapping: addr
	case -1:
		perror("fork()");
		exit(EXIT_FAILURE);

	case 0:  // child: increment shared integer, unmap the child's access and exit
		fprintf(stderr, "child: value = %d\n", *addr);
		(*addr)++;

		if (-1 == munmap(addr, sizeof(*addr))) {
			perror("munmap()");
			_exit(EXIT_FAILURE);
		}
		_exit(EXIT_SUCCESS);

	default:  // parent: wait for child to terminate
		if (-1 == wait(NULL)) {
			perror("wait()");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "parent: value = %d\n", *addr);
		if (-1 == munmap(addr, sizeof(int))) {
			perror("munmap()");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
