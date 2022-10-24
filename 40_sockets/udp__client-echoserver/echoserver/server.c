/*
  unix socket - server


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1241
*/

#define _DEFAULT_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h> /* open(), umask() */
#include <fcntl.h>    /* open() */
#include <signal.h>

#include <syslog.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

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

int
inet_bind(const char *service, int type, socklen_t *addrlen)
{
	return inet_passive_socket(service, type, addrlen, false, 0);
}

char*
inet_address_str(const struct sockaddr *addr, socklen_t addrlen,
		 char *addr_str, int addr_str_len)
{
	char host[NI_MAXHOST], service[NI_MAXSERV];

	if (0 == getnameinfo(addr, addrlen, host, NI_MAXHOST,
			     service, NI_MAXSERV, NI_NUMERICSERV)) {
		snprintf(addr_str, addr_str_len, "(%s, %s)", host, service);
	} else {
		snprintf(addr_str, addr_str_len, "(?UNKNOWN?)");
	}

	return addr_str;
}
/* --- */

int
main(int argc, char* argv[])
{
	int sockfd;
	ssize_t nread;
	socklen_t len;
	struct sockaddr_storage claddr;
	char buf[BUF_SIZE];
	char addr_str[IS_ADDR_STR_LEN];

	if (-1 == become_daemon(0)) {
		perror("become_daemon()");
		exit(EXIT_FAILURE);
	}

	sockfd = inet_bind(SERVICE, SOCK_DGRAM, NULL);
	if (-1 == sockfd) {
		syslog(LOG_ERR, "SERVER - could not create server socket (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	  receive datagrams and return copies to senders
	*/

	while (true) {
		len = sizeof(claddr);
		nread = recvfrom(sockfd, buf, BUF_SIZE, 0,
				 (struct sockaddr*) &claddr, &len);
		if (-1 == nread) {
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}

		if (nread != sendto(sockfd, buf, nread, 0, (struct sockaddr*) &claddr, len)) {
			syslog(LOG_WARNING, "SERVER - error echoing response to %s (%s)",
			       inet_address_str((struct sockaddr*) &claddr, len, addr_str, IS_ADDR_STR_LEN),
			       strerror(errno));
		}
	}

	fprintf(stderr, "SERVER - READY.\n");
	exit(EXIT_SUCCESS);
}
