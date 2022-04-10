/*
   demonstrate obtaining uname data, and dealing with struct utsname

   based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 230
 */

#ifdef __linux__
# define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>


int
main(int argc, char *argv[])
{
	struct utsname uts;

	if(-1 == uname(&uts)) {
		perror("uname() failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "Node name:\t%s\n", uts.nodename);
	fprintf(stderr, "System name:\t%s\n", uts.sysname);
	fprintf(stderr, "Release:\t%s\n", uts.release);
	fprintf(stderr, "Version:\t%s\n", uts.version);
	fprintf(stderr, "Machine:\t%s\n", uts.machine);
#ifdef _GNU_SOURCE
	fprintf(stderr, "Domain name:\t%s\n", uts.domainname);
#endif

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}

