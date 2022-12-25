/*
  Retrieving Socket Addresses (p.1263)

  The getsockname() and getpeername9) system calls return,
  respectively, the local address to which a socket is bound and the
  address of the peer socket to which the local socket is connected.

  For both calls, sockfd is a file descriptor referring to a socket,
  and addr is a pointer to a suitable sized buffer that is used to
  return a structure containing the socket address.

  usage:
    $ ./getsocknames.elf 27976 &
      [1] 32379


    $ netstat -a | egrep '(Address|27976)'
      Proto Recv-Q Send-Q Local Address       Foreign Address     State
      tcp        0      0 0.0.0.0:27976       0.0.0.0:*           LISTEN
      tcp        0      0 localhost:58158     localhost:27976     ESTABLISHED
      tcp        0      0 localhost:27976     localhost:58154     TIME_WAIT
      tcp        0      0 localhost:27976     localhost:58158     ESTABLISHED

  From the above output, we can see that the connected socket (fdconn)
  was bound to the ephemeral port 58158! The netstat command shows us
  information about all the targets created by the program, and allows
  us to confirm the port information for the two connected sockets,
  which are in the ESTABLISHED state.

  The program demonstrates:
  - create a listening socket, fdlisten
  - create a second socket, fdconn, which is used to send a connection
    request to the socket created
  - accept() on the listening socket in order to create a third
    socket, fdaccept, that is connected to the socket created in the
    previous step
  - Use calls to getsockname() and getpeername() to obtain the local
    and peer addresses for the two connected sockets, fdconn and
    fdaccept. After each of these calls, the program converts the
    socket address into printable form
  - Sleep for a few seconds so that we can run netstat in order to
    confirm the socket address information.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1265
*/

#define _DEFAULT_SOURCE /* former _BSD_SOURCE */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

// netdb.h implies at least:
//#define _POSIX_C_SOURCE 200112L
#include <netdb.h>   /* NI_MAXHOST, NI_MAXSERV, NI_NUMERICSERV */

#define IS_ADDR_STR_LEN 4096


/*
  returns a file descriptor on success, or -1 on error
*/
int inet_connect(const char *host, const char* service, int type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sockfd, res;

	fprintf(stderr, "CLIENT: %s() - start\n", __func__);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;  // allows IPv4 or IPv6
	hints.ai_socktype = type;
	hints.ai_addr = NULL;
	res = getaddrinfo(host, service, &hints, &result);
	if (0 != res) {
		errno = ENOSYS;
		return -1;
	}

	/*
	  walk through returned list until we find an address
	  structure that can be used to successfully connect a socket
	*/

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		// socket()
		fprintf(stderr, "CLIENT: %s() - socket()\n", __func__);
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (-1 == sockfd)
			continue;  // on error, try next address

		// connect()
		fprintf(stderr, "CLIENT: %s() - connect( %d, %ld, %d )\n", __func__, sockfd, (long) &rp->ai_addr, rp->ai_addrlen);
		if (-1 != connect(sockfd, rp->ai_addr, rp->ai_addrlen))
			break;  // success

		// failed: close this socket and try next address
		fprintf(stderr, "CLIENT: %s() - close(), FAILED\n", __func__);
		close(sockfd);
	}
	freeaddrinfo(result);

	return (NULL == rp) ? -1 : sockfd;
}
/* --- */

static int
inet_passive_socket(const char *service, int type,
		    socklen_t *addrlen, bool do_listen)
{
	int backlog;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sockfd = -1, optval, res;

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
		char *ptr = NULL;
		// can override 2nd argument with environment variable
		if (NULL != (ptr = getenv("LISTENQ"))) {
			backlog = atoi(ptr);
		}

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
main(int argc, char *argv[])
{
	int fdlisten, fdaccept, fdconn;
	socklen_t len;
	void *addr;   // size of socket address buffer
	char addr_str[IS_ADDR_STR_LEN];  // buffer for socket address

	if (2 != argc) {
		fprintf(stderr, "usage:\n$ %s service\n",
			argv[0]);
	}

	fprintf(stderr, "socket types:\n");
	fprintf(stderr, "\tSOCK_STREAM: '%d'\n", SOCK_STREAM);
	fprintf(stderr, "\tSOCK_DGRAM: '%d'\n", SOCK_DGRAM);
	fprintf(stderr, "\tSOCK_SEQPACKET: '%d'\n", SOCK_SEQPACKET);
	fprintf(stderr, "\tSOCK_RAW: '%d'\n", SOCK_RAW);
	fprintf(stderr, "\tSOCK_RDM: '%d'\n", SOCK_RDM);  /* reliable datagram layer, does not guarantee ordering */
	fprintf(stderr, "\tSOCK_PACKET: '%d'\n", SOCK_PACKET);

	fdlisten = inet_passive_socket(argv[1], SOCK_SEQPACKET, &len, true);
	if (-1 == fdlisten) {
		fprintf(stderr, "listen() failed\n");
		exit(EXIT_FAILURE);
	}

	fdconn = inet_connect(NULL, argv[1], SOCK_STREAM);
	if (-1 == fdconn) {
		fprintf(stderr, "connect() failed\n");
		exit(EXIT_FAILURE);
	}

	fdaccept = accept(fdlisten, NULL, NULL);
	if (-1 == fdaccept) {
		perror("accept() failed");
		exit(EXIT_FAILURE);
	}

	addr = malloc(len);
	if (NULL == addr) {
		perror("malloc() failed");
		exit(EXIT_FAILURE);
	}

	if (-1 == getsockname(fdconn, addr, &len)) {
		perror("getsockname(fdconn) failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "getsockname(fdconn): %s\n",
		inet_address_str(addr, len, addr_str, IS_ADDR_STR_LEN));
	if (-1 == getsockname(fdaccept, addr, &len)) {
		perror("getsockname(fdaccept) failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "getsockname(fdaccept): %s\n",
		inet_address_str(addr, len, addr_str, IS_ADDR_STR_LEN));

	if (-1 == getpeername(fdconn, addr, &len)) {
		perror("getpeername(fdconn) failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "getpeername(fdconn): %s\n",
		inet_address_str(addr, len, addr_str, IS_ADDR_STR_LEN));

	if (-1 == getpeername(fdaccept, addr, &len)) {
		perror("getpeername(fdaccept) failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "getpeername(fdaccept): %s\n",
		inet_address_str(addr, len, addr_str, IS_ADDR_STR_LEN));

	sleep(30);   // give some time to run netstat
	exit(EXIT_SUCCESS);
}
