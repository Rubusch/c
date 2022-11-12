/*
  tcp - concurrent server


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1244
*/

#define _DEFAULT_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>

#include "commons.h"

#include "tlpi/get_num.h"

static const char *LOG_FILE = "/tmp/ds.log";

/*
  returns 0 on success, -1 on error
*/
int
become_daemon(int flags)
{
	int maxfd, fd;

	switch (fork()) {  // become background process
	case -1: return -1;
	case 0: break;     // child falls through...
	default:
		return 0;  // ...while parent terminates
	}

	if (-1 == setsid()) { // become leader of new session
		return -1;
	}

	switch (fork()) {  // ensure we are not session leader
	case -1: return -1;
	case 0: break;
	default:
		sleep(1);
		return 0;
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
/* --- */

static int
inet_passive_socket(const char *service, int type,
		    socklen_t *addrlen, bool do_listen,
		    int backlog)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sockfd, optval, res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_socktype = type;
	hints.ai_family = AF_UNSPEC;  /* allows IPv4 or IPv6 */
	hints.ai_flags = AI_PASSIVE;  /* use wildcard IP address */

	// init 'result'
	res = getaddrinfo(NULL, service, &hints, &result);
	if (0 != res) {
		return -1;
	}

	/*
	  walk through returned list until we find an address
          structure that can be used to successfully create and bind a
          socket
	*/

	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (-1 == sockfd) {
			continue;  /* on error, try next address */
		}

		if (do_listen) {
			if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
					     &optval,
					     sizeof(optval))) {
				close(sockfd);
				freeaddrinfo(result);
				return -1;
			}
		}

		if (0 == bind(sockfd, rp->ai_addr, rp->ai_addrlen)) {
			break;  /* success */
		}

		/*
		  bind() failed: close this socket and try next address
		*/

		close(sockfd);
	}

	if (rp != NULL && do_listen) {
		if (-1 == listen(sockfd, backlog)) {
			freeaddrinfo(result);
			return -1;
		}
	}

	if (rp != NULL && addrlen != NULL) {
		*addrlen = rp->ai_addrlen;  /* return address structure size */
	}

	freeaddrinfo(result);
	return (rp == NULL) ? -1 : sockfd;
}
/* --- */

/*
  SIGCHLD handler to reap dead child processes
*/
static void
grim_reaper(int sig)
{
	int saved_errno; // saved errno in case changed here

	saved_errno = errno;
	while (0 < waitpid(-1, NULL, WNOHANG))
		continue;

	errno = saved_errno;
}

/*
  handle a client request: copy socket input back to socket
*/

static void
handle_request(int cfd)
{
	char buf[BUF_SIZE];
	ssize_t nread;

	while (0 < (nread = read(cfd, buf, BUF_SIZE))) {
		if (nread != write(cfd, buf, nread)) {
			syslog(LOG_ERR, "write() failed: %s",
			       strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (-1 == nread) {
		syslog(LOG_ERR, "error from read(): %s",
		       strerror(errno));
		exit(EXIT_FAILURE);
	}
}


int
main(int argc, char *argv[])
{
	int lfd, cfd;
	struct sigaction sa;

	if (-1 == become_daemon(0)) {
		perror("become_daemon()");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = grim_reaper;
	if (-1 == sigaction(SIGCHLD, &sa, NULL)) {
		syslog(LOG_ERR, "error from sigaction(): %s",
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	// inet_listen()
	lfd = inet_passive_socket(SERVICE, 10, NULL, true, 0);

	if (-1 == lfd) {
		syslog(LOG_ERR, "could not create server socket (%s)",
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (true) {
		cfd = accept(lfd, NULL, NULL); // wait for connection
		if (-1 == cfd) {
			syslog(LOG_ERR, "failure in accept(): %s",
			       strerror(errno));
			exit(EXIT_FAILURE);
		}

		/*
		  handle each client request in a new child process
		*/

		switch (fork()) {
		case -1: /* error */
			syslog(LOG_ERR, "can't create child (%s)",
			       strerror(errno));
			close(cfd);  // give up on this client
			break;

		case 0: /* child */
			close(lfd);  // unneeded copy of listening socket
			handle_request(cfd);
			_exit(EXIT_SUCCESS);

		default: /* parent */
			close(cfd);  // unneeeded copy of connected socket
			break;
		}
	}
}
