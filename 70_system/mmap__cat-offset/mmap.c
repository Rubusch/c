/*
  mmap demo

  usage:
  $ ./mmap.elf <filename>
  ...


  Demonstrates the use of mmap() to create a private file
  mapping. This program is a simple version of cat. It maps the
  (entire) file named in its command-line argument, and then writes
  the contents of the mapping to standard output.

  Extended demo dealing with offset and length as additional
  arguments.


  Based on manpage mmap
*/

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define handle_error(msg)					\
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int
main(int argc, char *argv[])
{
	char *addr;
	int fd;
	struct stat sb;
	off_t offset, pa_offset;
	size_t length;
	ssize_t ret;

	if (argc < 3 || argc > 4) {
		fprintf(stderr, "%s <filename> <offset> [length]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
		handle_error("fstat");

	offset = atoi(argv[2]);
	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
        /* offset for mmap() must be page aligned */

	if (offset >= sb.st_size) {
		fprintf(stderr, "offset is past end of file\n");
		exit(EXIT_FAILURE);
	}

	if (argc == 4) {
		length = atoi(argv[3]);
		if (offset + length > sb.st_size)
			length = sb.st_size - offset;
		/* can't display bytes past end of file */

	} else {    /* No length arg ==> display to end of file */
		length = sb.st_size - offset;
	}

	addr = mmap(NULL, length + offset - pa_offset, PROT_READ,
		    MAP_PRIVATE, fd, pa_offset);
	if (addr == MAP_FAILED)
		handle_error("mmap");

	ret = write(STDOUT_FILENO, addr + offset - pa_offset, length);
	if (ret != length) {
		if (ret == -1)
			handle_error("write");

		fprintf(stderr, "partial write");
		exit(EXIT_FAILURE);
	}

	munmap(addr, length + offset - pa_offset);
	close(fd);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
