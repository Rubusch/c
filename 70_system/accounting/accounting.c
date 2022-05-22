/*
  accounting, pt1/2: the accounting file generator

  usage:
  $ sudo su
  # touch pacct
  # ./accounting.elf pacct
    process accounting enabled
    READY.


  Normally, process accounting is enabled at each system restart by
  placing appropriate commands in the system boot scripts.

  To enable process accounting, we supply the pathname of an existing
  regular file in "acctfile". A typical pathname for the accounting
  file is...
    /var/log/pacct
  or
    /usr/account/pacct

  To disable process accounting, we specify acctfile as NULL.

  Once process accounting is enabled, an acct record "struct acct" is
  written to the accounting file as each process terminates. The acct
  structure is defined in <sys/acct.h>


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 592
*/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/acct.h>


int
main(int argc, char *argv[])
{
	if (2 < argc) {
		fprintf(stderr, "usage:\n$ %s [file]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (-1 == acct(argv[1])) {
		fprintf(stderr, "acct() failed\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "process accounting %s\n",
		(argv[1] != NULL) ? "enabled" : "disabled");

	printf("READY.\n");
	exit(EXIT_SUCCESS);
}
