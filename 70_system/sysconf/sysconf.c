/*
   display some system limits via sysconf

   based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 216
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

static void
sysconf_print(const char *msg, int name)
{
	long lim;

	errno = 0;
	lim = sysconf(name);
	if (lim != -1) {  // call succeeded, limit determinate
		fprintf(stderr, "%s %ld\n", msg, lim);
	} else {
		if (0 == errno) { // call succeeded, limit indeterminate
			fprintf(stderr, "%s (indeterminate)\n", msg);
		} else {
			fprintf(stderr, "sysconf %s", msg);
			exit(EXIT_FAILURE);
		}
	}
}


int
main(int argc, char *argv[])
{
	sysconf_print("_SC_ARG_MAX:\t\t", _SC_ARG_MAX);
	sysconf_print("_SC_LOGIN_NAME_MAX:\t", _SC_LOGIN_NAME_MAX);
	sysconf_print("_SC_OPEN_MAX:\t\t", _SC_OPEN_MAX);
	sysconf_print("_SC_NGROUPS_MAX:\t", _SC_NGROUPS_MAX);
	sysconf_print("_SC_PAGESIZE:\t\t", _SC_PAGESIZE);
	sysconf_print("_SC_RTSIG_MAX:\t\t", _SC_RTSIG_MAX);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}

