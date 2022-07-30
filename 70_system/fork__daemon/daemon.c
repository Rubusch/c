/*
  daemon - shows how to implement a daemon

  the example demonstrates a (mocked) logger..
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h> /* open(), umask() */
#include <fcntl.h>    /* open() */
#include <signal.h>
#include <unistd.h> /* file operations */

#include "daemon.h"


/*
  returns 0 on success, -1 on error
*/
int
become_daemon(int flags)
{
	int maxfd, fd;

	switch (fork()) { // become background process
	case -1: return -1;
	case 0: break;                // child falls through...
	default: _exit(EXIT_SUCCESS); // ...while parent terminates
	}

	if (-1 == setsid()) { // become leader of new session
		return -1;
	}

	switch (fork()) { // ensure we are not session leader
	case -1: return -1;
	case 0: break;
	default: _exit(EXIT_SUCCESS);
	}

	if (!(flags & BD_NO_UMASK0)) {
		umask(0); // clear file mode creation mask
	}

	if (!(flags & BD_NO_CHDIR)) {
		chdir("/"); // change to root directory
	}

	if (!(flags & BD_NO_CLOSE_FILES)) { // close all open files
		maxfd = sysconf(_SC_OPEN_MAX);
		if (-1 == maxfd) { // limit is indeterminate...
			maxfd = BD_MAX_CLOSE; // so take a guess
		}

		for (fd = 0; fd < maxfd; fd++) {
			close(fd);
		}
	}

	if (!(flags & BD_NO_REOPEN_STD_FDS)) {
		close(STDIN_FILENO); // reopen standard fd's to /dev/null

		fd = open(LOG_FILE, O_RDWR);
		if (fd != STDIN_FILENO) { // 'fd' should be 0
			return -1;
		}

		if (STDOUT_FILENO != dup2(STDIN_FILENO, STDOUT_FILENO)) {
			return -1;
		}

		if (STDERR_FILENO != dup2(STDIN_FILENO, STDERR_FILENO)) {
			return -1;
		}
	}

	return 0;
}
