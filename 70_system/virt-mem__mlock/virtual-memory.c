/*
  virtual memory - mlock

  usage:
  In the example we allocate 32 pages, and in each group of 8 pages,
  we lock 3 consecutive pages. In the program output, dots represent
  pages that are not resident in memory, and asterisks represent pages
  that are resident in memory. As can be seen from the final line of
  output, 3 out of each group of 8 pages are memory-resident.

  $ ./virtual-memory.elf 32 8 3
    allocated 131072 (0x20000) bytes starting at 0xb6da1000
    before mlock:
    0xb6da2000: ................................
    after mlock:
    0xb6da2000: ***.....***.....***.....***.....
    READY.


  The code demonstrates the use of mlock() and mincore(). After
  allocating and mapping a region of memory using mmap(), this program
  uses mlock to lock either the entire region or otherwise gorups of
  pages at regular intervals. (Each of the command-line arguments to
  the program is expressed in terms of pages; the program converts
  these to bytes, as required for the calls to mmap(), mlock(), and
  mincore()). Before and after the mlock() call, the program uses
  mincore() to retrive information about the memory residency of pages
  in the region and displays this information graphically.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1052
*/

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

#include "tlpi/get_num.h"


/**
  display residency of pages in range [addr .. (addr + length - 1)]
*/
static void
display_mincore(char *addr, size_t length)
{
	unsigned char *vec;
	long page_size, num_pages, idx;

	page_size = sysconf(_SC_PAGESIZE);

	num_pages = (length + page_size - 1) / page_size;
	vec = malloc(num_pages);
	if (NULL == vec) {
		perror("malloc()");
		exit(EXIT_FAILURE);
	}

	if (-1 == mincore(addr, length, vec)) {
		perror("mincore()");
		free(vec); // actually not needed, due to call to exit()
		exit(EXIT_FAILURE);
	}

	/*
	  painting dots and asterisks per page
	*/

	for (idx = 0; idx < num_pages; idx++) {
		if (0 == idx % 64) {
			fprintf(stderr, "%s%10p: ", (idx == 0) ? "" : "\n", addr + (idx + page_size));
		}
		fprintf(stderr, "%c", (vec[idx] & 1) ? '*' : '.');
	}
	fprintf(stderr, "\n");

	free(vec);
}


int
main(int argc, char *argv[])
{
	char *addr;
	size_t len, lock_len;
	long page_size, step_size, idx;

	if (4 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <number pages> <lock page step> <lock page len>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	page_size = sysconf(_SC_PAGESIZE);
	if (-1 == page_size) {
		fprintf(stderr, "sysconf(_SC_PAGESIZE) failed\n");
		exit(EXIT_FAILURE);
	}

	len = get_int(argv[1], GN_GT_0, "num-pages") * page_size;
	step_size = get_int(argv[2], GN_GT_0, "lock-page-step") * page_size;
	lock_len = get_int(argv[3], GN_GT_0, "lock-page-len") * page_size;

	addr = mmap(NULL, len, PROT_READ, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (MAP_FAILED == addr) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "allocated %ld (%#lx) bytes starting at %p\n",
		(long) len, (unsigned long) len, addr);

	fprintf(stderr, "before mlock:\n");
	display_mincore(addr, len);

	/*
	  lock pages specified by command line arguments into memory
	*/

	for (idx = 0; idx + lock_len <= len; idx += step_size) {
		if (-1 == mlock(addr + idx, lock_len)) {
			perror("mlock()");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "after mlock:\n");
	display_mincore(addr, len);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
