// filesize.c
/*
  demonstrate using O_DIRECT to bypass the buffer cache

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 247
//*/

#define _GNU_SOURCE  /* obtain O_DIRECT definition from <fcntl.h> */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h> /* lseek() */
#include <malloc.h> /* memalign() */

#include "tlpi/get_num.h"

void usage(const char *name)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "$ %s <file> <lenght> [<offset> <alignment>]\n", name);
	fprintf(stderr, "e.g.\n");
	fprintf(stderr, "$ %s ./text.txt 512\n", name);

	fprintf(stderr, "$ %s ./text.txt 256\n", name);
// fails: length is not multiple of 512

	fprintf(stderr, "$ %s ./text.txt 512 1\n", name);
// fails: offset is not a multiple of 512

	fprintf(stderr, "$ %s ./text.txt 4096 8192 512\n", name);
	fprintf(stderr, "$ %s ./text.txt 4096 8192 512 256\n", name);
// fails: alignment is not a multiple of 512

	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int fd;
	ssize_t nread;
	size_t len, align;
	off_t offset;
	char *buf;

	if (argc < 3) {
		usage(argv[0]);
	}

	len = get_long(argv[2], GN_ANY_BASE, "length");
	offset = (argc > 3) ? get_long(argv[3], GN_ANY_BASE, "offset") : 0;
	align = (argc > 4) ? get_long(argv[4], GN_ANY_BASE, "alignment") : 4096;

	fd = open(argv[1], O_RDONLY | O_DIRECT); // demonstrates turn off cache
	if (-1 == fd) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

	/*
	  memalign() allocates a block of memory aligned on an address
	  that is a multiple of its first argument. By specifying this
	  argument as

	    2 * 'alignment'

	  and then adding 'alignment' to the returned pointer, we
	  ensure that 'buf' is aligned on a non-power-of-two multiple
	  of 'alignment'. We do this to ensure that if, for example,
	  we ask 'alignment'. We do this to ensure that if, for
	  example, we ask for a 256-byte aligned buffer, we don't
	  accidentally get a buffer that is also aligned on a 512-byte
	  boundary
	*/
	buf = memalign(align * 2, len + align);
	if (NULL == buf) {
		perror("memalign() failed");
		exit(EXIT_FAILURE);
	}

	buf += align;

	if (-1 == lseek(fd, offset, SEEK_SET)) {
		perror("lseek() failed");
		exit(EXIT_FAILURE);
	}

	nread = read(fd, buf, len);
	if (-1 == nread) {
		perror("read() failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "read %ld bytes\n", (long) nread);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
