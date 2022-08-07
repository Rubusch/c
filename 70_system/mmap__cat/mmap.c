/*
  mmap demo

  usage:
  $ ./mmap.elf <filename>
    ...

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


  Demonstrates the use of mmap() to create a private file
  mapping. This program is a simple version of cat. It maps the
  (entire) file named in its command-line argument, and then writes
  the contents of the mapping to standard output.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1022
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
	char *addr = MAP_FAILED;
	int fd;
	struct stat sb;

	if (argc != 2) {
		fprintf(stderr, "usage:\n"
			"$ %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	if (-1 == fd) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	/*
	  obtain the size of the file and use it to specify the size
	  of the mapping and the size of the buffer to be written
	*/
	if (-1 == fstat(fd, &sb)) {
		perror("fstat()");
		exit(EXIT_FAILURE);
	}

	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	if (sb.st_size != write(STDOUT_FILENO, addr, sb.st_size)) {
		fprintf(stderr, "partial/failed write\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
