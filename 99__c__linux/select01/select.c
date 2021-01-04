// select.c
/*
  select example of the manpage
*/


#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


/*
  watch stdin (fd 0) to see when it has input, wait up to five seconds
  then don't rely on the value of "tv"
*/
int main(int argc, char** argv)
{
	// 0. define a set
	int nsecs = 3;
	fd_set rset;
	struct timeval timeout;
	int retval;

	fprintf(stdout, "select example - either press <ENTER> to give some\ninput on the observed readfds or let it timeout in %dsec\n", nsecs);

	// 1. zero the set
	fprintf(stdout, "\tFD_ZERO() - zero the set\n\n");
	FD_ZERO(&rset);

	// 2. set up a new set, with user inputstream
	fprintf(stdout, "\tFD_SET(0, &rset) - set up a new element to set\n\n");
	FD_SET(0, &rset);

	timeout.tv_sec = nsecs; // secs and..
	timeout.tv_usec = 0; // ..msecs to wait

	/*
	  3. call select(nfds, readfds, writefds, exceptfds, timeout)

	  nfds      - highest-numbered file descriptor in any of the three sets
	  readfds   - read filedescriptor (will be watched for reading)
	  writefds  - write filedescriptor (pointer on write stream)
	  exceptfds - exception filedescriptor (pointer on exception stream)
	  timeout   - an upper timeout until select returns
	*/
	fprintf(stdout, "\tselect(1, &rset, NULL, ..., timeout) - call to select() or timeout %d secs\n", nsecs);
	retval = select(1, &rset, NULL, NULL, &timeout);
	if (-1 == retval) {
		perror("select error");
	} else if (retval) {
		fprintf(stdout, "result: data is available\n");
		if (FD_ISSET(0, &rset)) {
			fprintf(stdout, "\tFD_ISSET(0, &rset) is true now\n\n");
		}

	} else {
		fprintf(stdout, "\nresult: no data within five seconds\n\n");

	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

