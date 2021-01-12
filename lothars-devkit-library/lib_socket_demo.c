/*
  socket demo snippets

  parts and pieces of socket implementations

  Lothar Rubusch

  ---
  References
  Unix Network Programming, Stevens
*/

#include "lib_socket_demo.h"


/*
  connect_nonb.c
*/
int connect_nonb(int fd_sock, const struct sockaddr *saptr, socklen_t salen, int nsec)
{
	int flags, res, error;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;

	flags = lothars__fcntl(fd_sock, F_GETFL, 0);
	lothars__fcntl(fd_sock, F_SETFL, flags | O_NONBLOCK);

	error = 0;
	if (0 > (res = connect(fd_sock, saptr, salen))) {
		if (errno != EINPROGRESS) {
			return -1;
		}
	}

	// do whatever we want while the connect is taking place
	if (0 == res) {
		goto done; // connect completed immediately
	}

	FD_ZERO(&rset);
	FD_SET(fd_sock, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;

	if (0 == (res = lothars__select(fd_sock+1, &rset, &wset, NULL, (nsec ? &tval : NULL)))) {
		close(fd_sock);  // timeout
		errno = ETIMEDOUT;
		return -1;
	}

	if (FD_ISSET(fd_sock, &rset) || FD_ISSET(fd_sock, &wset)) {
		len = sizeof(error);

		if (0 > getsockopt(fd_sock, SOL_SOCKET, SO_ERROR, &error, &len)) {
			return -1; // Solaris pending error
		}
	} else {
		err_quit("select error: fd_sock not set");
	}

done:
	lothars__fcntl(fd_sock, F_SETFL, flags); // restore file status flags

	if (error) {
		close(fd_sock); // just in case
		errno = error;
		return -1;
	}
	return 0;
}


/*
  connect_timeo.c

  TODO demo code snippet?                                 
*/
// some dummy signal routine
static void connect_alarm(int signo)
{
	return; // just interrupt the connect()
}
int connect_timeo(int fd_sock, const struct sockaddr *saptr, socklen_t salen, int nsec)
{
	Sigfunc *sigfunc;
	int res;

	sigfunc = lothars__signal(SIGALRM, connect_alarm);
	if (alarm(nsec) != 0) {
		err_msg("connect_timeo: alarm was already set");
	}

	if (0 > (res = connect(fd_sock, saptr, salen))) {
		close(fd_sock);
		if (errno == EINTR) {
			errno = ETIMEDOUT;
		}
	}

	alarm(0); // turn off the alarm
	lothars__signal(SIGALRM, sigfunc); // restore previous signal handler

	return res;
}


void lothars__connect_timeo(int fd, const struct sockaddr *sa, socklen_t salen, int sec)
{
	if (0 > connect_timeo(fd, sa, salen, sec)) {
		err_sys("%s() error", __func__);
	}
}



/*
  daemon_init.c
*/

//int daemon_proc; // defined in error.c
int daemon_init(const char *pname, int facility)
{
	int  idx;
	pid_t pid;

	if (0 > (pid = lothars__fork())) {
		return -1;
	} else if (pid) {
		exit(EXIT_SUCCESS); // parent terminates
	}

	// child 1 continues...

	if (0 > setsid()) { // become session leader
		return -1;
	}

	lothars__signal(SIGHUP, SIG_IGN);
	if (0 > (pid = lothars__fork())) {
		return -1;
	} else if (pid) {
		exit(EXIT_SUCCESS); // child 1 terminates
	}

	// child 2 continues...

//	daemon_proc = 1; // for err_XXX() functions

	chdir("/"); // change working directory

	// close off file descriptors
	for (idx = 0; idx < MAXFD; ++idx) {
		close(idx);
	}

	// redirect stdin, stdout, and stderr to /dev/null
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

//	openlog(pname, LOG_PID, facility); // TODO rm

	return 0; // success
}

/*
  daemon_inetd.c
*/
int daemon_proc; // defined in error.c for syslog
void daemon_inetd(const char *pname, int facility)
{
//	daemon_proc = 1;  // for our err_XXX() functions
	fprintf(stdout, "setting the eudaimonia to 1 and put this process to the background\n");
//	openlog(pname, LOG_PID, facility); // use this when logging
	fprintf(stdout, "loggging %s: %d\n", pname, facility);
}


/*
  dg_cli.c - prototype snippet
*/
void dg_cli(FILE *fp, int fd_sock, const struct sockaddr *pservaddr, socklen_t servlen)
{
	int n_bytes;
	char sendline[MAXLINE], recvline[MAXLINE + 1];

	while (lothars__fgets(sendline, MAXLINE, fp) != NULL) {
		lothars__sendto(fd_sock, sendline, strlen(sendline), 0, pservaddr, servlen);
		n_bytes = lothars__recvfrom(fd_sock, recvline, MAXLINE, 0, NULL, NULL);
		recvline[n_bytes] = 0; // null terminate
		lothars__fputs(recvline, stdout);
	}
}


/*
  dg_echo.c - prototype snippet
*/
void dg_echo(int fd_sock, struct sockaddr *pcliaddr, socklen_t clilen)
{
	int n_bytes;
	socklen_t len;
	char msg[MAXLINE]; memset(msg, '\0', sizeof(msg));

	len = clilen;
	while (1) {
//		len = clilen; // TODO check
		n_bytes = lothars__recvfrom(fd_sock, msg, MAXLINE, 0, pcliaddr, &len);
		lothars__sendto(fd_sock, msg, n_bytes, 0, pcliaddr, len);
	}
}



/*
  demo snippet - family to level
*/
int lothars__family_to_level(int family)
{
	switch (family) {
	case AF_INET:
		return IPPROTO_IP;

#ifdef IPV6
	case AF_INET6:
		return IPPROTO_IPV6;
#endif

	default:
		err_sys("%s() error", __func__);
		return -1;
	}
}


/*
  The getsockname() function shall retrieve the locally-bound name of
  the specified socket, store this address in the sockaddr structure
  pointed to by the address argument, and store the length of this
  address in the object pointed to by the address_len argument.

  #include <sys/socket.h>

  @fd_sock: The socket descriptor.

  This wrapper returns the address family to the socket name, or
  socket descriptor, respectively.
*/
int lothars__socket_to_family(int fd_sock)
{
	struct sockaddr_storage ss;
	socklen_t len;

	len = sizeof(ss);
	if (0 > getsockname(fd_sock, (struct sockaddr *) &ss, &len)) {
		err_sys("%s() error", __func__);
		return -1;
	}
	return ss.ss_family;
}


/*
  demo snippet - gf time
*/

char* gf_time(void)
{
	struct timeval tv;
	time_t time;
	static char  str[30];
	char *ptr;

	if (0 > gettimeofday(&tv, NULL)) {
		err_sys("%s() error", __func__);
	}
	time = tv.tv_sec; // POSIX says tv.tv_sec is time_t; some BSDs don't agree.
	ptr = ctime(&time);
	strcpy(str, &ptr[11]);
	// Fri Sep 13 00:00:00 1986\n\0
	// 0123456789012345678901234 5
	snprintf(str+8, sizeof(str)-8, ".%06ld", tv.tv_usec);

	return str;
}


/*
   demo snippet - host server
*/


/*
  pt. 1/2
*/
struct addrinfo* host_serv(const char *host, const char *serv, int family, int socktype)
{
	struct addrinfo hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; // always return canonical name
	hints.ai_family = family;  // AF_UNSPEC, AF_INET, AF_INET6, etc.
	hints.ai_socktype = socktype; // 0, SOCK_STREAM, SOCK_DGRAM, etc.

	if (0 != getaddrinfo(host, serv, &hints, &res)) {
		return NULL;
	}

	return res; // return pointer to first on linked list
}


/*
  pt. 2/2

  there is no easy way to pass back the integer return code from
  getaddrinfo() in the function above, short of adding another
  argument that is a pointer, so the easiest way to provide the
  wrapper function is just to duplicate the simple function as we do
  here
*/
struct addrinfo* lothars__host_serv(const char *host, const char *serv, int family, int socktype)
{
	struct addrinfo hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; // always return canonical name
	hints.ai_family = family;  // 0, AF_INET, AF_INET6, etc.
	hints.ai_socktype = socktype; // 0, SOCK_STREAM, SOCK_DGRAM, etc

	int eai;
	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("_host_serv error for %s, %s: %s"
			 , (host == NULL) ? "(no hostname)" : host
			 , (serv == NULL) ? "(no service name)" : serv
			 , gai_strerror(eai));
	}

	return res; // return pointer to first on linked list
}



/*
  demo snippet - my addrs
*/

/*
  pt. 1/2
*/
char** my_addrs(int *addrtype)
{
	struct hostent *hptr = NULL;
	struct utsname myname;

	if (0 > uname(&myname)) {
		return NULL;
	}

	if (NULL == (hptr = gethostbyname(myname.nodename))) {
		return NULL;
	}

	*addrtype = hptr->h_addrtype;

	return hptr->h_addr_list;
}


/*
  pt 2/2
*/
char** lothars__my_addrs(int *pfamily) // TODO name!             
{
	char **pptr;
	if (NULL == (pptr = my_addrs(pfamily))) {
		err_sys("%s() error", __func__);
	}

	return pptr;
}



/*
  demo snippets - worker for echo server (general, text)

  it reads data from the lcient and echoes it back to the client
*/

void worker__echo_serv(int fd_sock)
{
	ssize_t n_bytes = -1;
	char buf[MAXLINE];

again:
	memset(buf, '\0', sizeof(buf));

	while (0 < (n_bytes = read(fd_sock, buf, sizeof(buf)))) {
		fprintf(stdout, "tcp:\t%s", buf);
		lothars__writen(fd_sock, buf, n_bytes);
	}

	if (n_bytes < 0) {
		if (errno == EINTR) {
			fprintf(stdout, "%s() interrupt caught\n", __func__);
			goto again;
		} else {
			err_sys("%s() read error", __func__);
		}
	}
}


/*
  demo snippets - worker for echo client (general, text)

  it reads a line of text from standard input, writes it to the
  server, reads back the server's echo of the line and outputs the
  echoed line to standard output
*/

void worker__echo_cli(FILE *fp, int fd_sock)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	memset(sendline, '\0', sizeof(sendline));
	memset(recvline, '\0', sizeof(recvline));
	while (NULL != lothars__fgets(sendline, sizeof(sendline), fp)) {
		lothars__writen(fd_sock, sendline, strlen(sendline));
		if (0 == lothars__readline_fd(fd_sock, recvline, sizeof(recvline)))
			err_quit("%s(): server terminated prematurely\n", __func__);

		lothars__fputs(recvline, stdout);
	}
}



/*
  demo snippets - worker for echo server (text)

  a.k.a. str_echo()
*/
/*
// TODO test                
void worker__echo_serv_str(int fd_sock)
{
	long arg1, arg2;
	ssize_t n_bytes;
	char line[MAXLINE];

	while (1) {
		memset(line, '\0', sizeof(line));

		if (0 == (n = lothars__readline(fd_sock, line, sizeof(line)))) {
			fprintf(stdout, "connection closed by other end\n");
			return;
		}

		if (2 == sscanf(line, "%ld%ld", &arg1, &arg2))
			snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
		else
			snprintf(line, sizeof(line), "input error\n");
		n_bytes = strlen(line);
		lothars__writen(fd_sock, line, n_bytes);
	}
}
// */


/*
  demo snippets - worker for echo server (binary)

  worker implementation for an echo server, answering on binary messages
  NB: byte order on server and client must be identical!

  a.k.a. str_echo()
*/

#ifndef DEVKIT_ECHO_CLIENT_SERVER_DEFINITIONS
#define DEVKIT_ECHO_CLIENT_SERVER_DEFINITIONS
/* this should go into the header, kept here just for completion */
struct args {
	long arg1, arg2;
};
struct result {
	long sum;
};
#endif /* DEVKIT_ECHO_CLIENT_SERVER_DEFINITIONS */
void worker__echo_serv_bin(int fd_sock)
{
	ssize_t nbytes;
	struct args args;
	struct result result;

	while (1) {
		if (0 == (nbytes = lothars__readn(fd_sock, &args, sizeof(args)))) {
			fprintf(stdout, "connection closed by other end\n");
			return;
		}
		result.sum = args.arg1 + args.arg2;
		lothars__writen(fd_sock, &result, sizeof(result));
	}
}


/*
  demo snippets - worker for echo client (binary)

  a.k.a str_cli()
*/

#ifndef DEVKIT_ECHO_CLIENT_SERVER_DEFINITIONS
#define DEVKIT_ECHO_CLIENT_SERVER_DEFINITIONS
/* this should go into the header, kept here just for completion */
struct args {
	long arg1, arg2;
};
struct result {
	long sum;
};
#endif /* DEVKIT_ECHO_CLIENT_SERVER_DEFINITIONS */
void worker__echo_cli_bin(FILE *fp, int fd_sock)
{
	char line[MAXLINE];
	struct args args;
	struct result result;

	while (NULL != lothars__fgets(line, sizeof(line), fp)) {
		if (2 != sscanf(line, "%ld%ld", &args.arg1, &args.arg2)) {
			fprintf(stdout, "invalid input: %s", line);
			continue;
		}
		lothars__writen(fd_sock, &args, sizeof(args));
		if (0 == lothars__readn(fd_sock, &result, sizeof(result)))
			err_quit("%s(): server terminated prematurely");
		fprintf(stdout, "%ld\n", result.sum);
	}
}


/*
  demo snippets - tcp_connect.c
*/

/*
*/
int lothars_tcp_connect(const char *host, const char *serv)
{
	int fd_sock, eai;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("tcp_connect error for %s, %s: %s", host, serv, gai_strerror(eai));
	}

	ressave = res;

	do {
		if (0 > (fd_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol))) {
			continue; // ignore this one
		}

		if (0 == connect(fd_sock, res->ai_addr, res->ai_addrlen)) {
			break;  // success
		}

		lothars__close(&fd_sock); // ignore this one
	} while (NULL != (res = res->ai_next));

	if (NULL == res) { // errno set from final connect()
		err_sys("%s() error for %s, %s", __func__, host, serv);
	}

	freeaddrinfo(ressave);

	return fd_sock;
}


/*
  demo snippets - tcp_listen.c
*/

int lothars__tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int listenfd, eai;
	const int  on = 1;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("tcp_listen error for %s, %s: %s", host, serv, gai_strerror(eai));
	}

	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (0 > listenfd) {
			continue; // error, try next one
		}
		lothars__setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (0 == bind(listenfd, res->ai_addr, res->ai_addrlen)) {
			break; // success
		}

		lothars__close(&listenfd); // bind error, close and try next one
	} while (NULL != (res = res->ai_next));

	if (NULL == res) { // errno from final socket() or bind()
		err_sys("%s() error for %s, %s", __func__, host, serv);
	}

	lothars__listen(listenfd, LISTENQ);

	if (addrlenp) {
		*addrlenp = res->ai_addrlen; // return size of protocol address
	}

	freeaddrinfo(ressave);

	return listenfd;
}



/*
  demo snippet - tv_sub
*/

void tv_sub(struct timeval *out, struct timeval *in)
{
	if (0 > (out->tv_usec -= in->tv_usec)) { // out -= in
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}



/*
  udp_client.c
*/
int lothars__udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t *lenp)
{
	int fd_sock, eai;
	struct addrinfo hints, *res=NULL, *ressave=NULL;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("udp_client error for %s, %s: %s", host, serv, gai_strerror(eai));
	}
	ressave = res;

	do {
		fd_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd_sock >= 0)
			break;  // success
	} while (NULL != (res = res->ai_next));

	if (res == NULL) { // errno set from final socket()
		err_sys("%s() error for %s, %s", __func__, host, serv);
	}

	*saptr = lothars__malloc(res->ai_addrlen);
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*lenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return fd_sock;
}


/*
  demo snippets - udp_connect.c
*/

/*
  pt 1/2
*/
int udp_connect(const char *host, const char *serv)
{
	int fd_sock, eai;
	struct addrinfo hints, *res=NULL, *ressave=NULL;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("udp_connect error for %s, %s: %s", host, serv, gai_strerror(eai));
	}

	ressave = res;

	do {
		if (0 > (fd_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol))) {
			continue; // ignore this one
		}

		if (0 == connect(fd_sock, res->ai_addr, res->ai_addrlen)) {
			break;  // success
		}

		lothars__close(&fd_sock); // ignore this one
	} while (NULL != (res = res->ai_next));

	if (res == NULL) { // errno set from final connect()
		err_sys("%s() error for %s, %s", __func__, host, serv);
	}

	freeaddrinfo(ressave);

	return fd_sock;
}


/*
  pt 2/2
*/
int lothars__udp_connect(const char *host, const char *serv)
{
	int res;
	if (0 > (res = udp_connect(host, serv))) {
		err_quit("udp_connect error for %s, %s: %s", host, serv, gai_strerror(-res));
	}
	return res;
}



/*
  demo snippet - udp_server.c
*/
int lothars__udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
	int fd_sock, eai;
	struct addrinfo hints, *res=NULL, *ressave=NULL;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("udp_server error for %s, %s: %s", host, serv, gai_strerror(eai));
	}

	ressave = res;

	do {
		if (0 > (fd_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol))) {
			continue; // error - try next one
		}

		if (bind(fd_sock, res->ai_addr, res->ai_addrlen) == 0) {
			break; // success
		}

		lothars__close(&fd_sock);  // bind error - close and try next one
	} while (NULL != (res = res->ai_next));

	if (res == NULL) { // errno from final socket() or bind()
		err_sys("%s() error for %s, %s", __func__, host, serv);
	}

	if (addrlenp) {
		*addrlenp = res->ai_addrlen; // return size of protocol address
	}

	freeaddrinfo(ressave);

	return fd_sock;
}



/*
  demo snippet - writable_timeo
*/

/*
  pt 1/2
 */
int writable_timeo(int fd, int sec)
{
	fd_set wset;
	struct timeval tv;

	FD_ZERO(&wset);
	FD_SET(fd, &wset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return select(fd+1, NULL, &wset, NULL, &tv);
	// > 0 if descriptor is writable
}


/*
  pt 2/2
*/
int lothars__writable_timeo(int fd, int sec)
{
	int  bytes;

	if (0 > (bytes = writable_timeo(fd, sec))) {
		err_sys("%s() error", __func__);
	}
	return bytes;
}


/*
  sock_bind_wild.c
*/
int sock_bind_wild(int fd_sock, int family)
{
	socklen_t len;

	switch (family) {
	case AF_INET:
	{
		struct sockaddr_in sin;

		bzero(&sin, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_port = htons(0); // bind ephemeral port

		if (0 > bind(fd_sock, (struct sockaddr *) &sin, sizeof(sin))) {
			return -1;
		}

		len = sizeof(sin);

		if (0 > getsockname(fd_sock, (struct sockaddr *) &sin, &len)) {
			return -1;
		}
		return sin.sin_port;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 sin6;

		bzero(&sin6, sizeof(sin6));
		sin6.sin6_family = AF_INET6;
		sin6.sin6_addr = in6addr_any;
		sin6.sin6_port = htons(0); // bind ephemeral port

		if (0 > bind(fd_sock, (struct sockaddr *) &sin6, sizeof(sin6))) {
			return -1;
		}

		len = sizeof(sin6);

		if (0 > getsockname(fd_sock, (struct sockaddr *) &sin6, &len)) {
			return -1;
		}

		return sin6.sin6_port;
	}
#endif
	}
	return -1;
}


int lothars__sock_bind_wild(int fd_sock, int family)
{
	int port;

	if (0 > (port = sock_bind_wild(fd_sock, family))) {
		err_sys("sock_bind_wild error");
	}

	return port;
}



/*
  sock_cmp_addr.c
*/
int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
	if (sa1->sa_family != sa2->sa_family) {
		return -1;
	}

	switch (sa1->sa_family) {
	case AF_INET:
	{
		return memcmp(&((struct sockaddr_in *) sa1)->sin_addr,
			      &((struct sockaddr_in *) sa2)->sin_addr,
			      sizeof(struct in_addr));
	}

#ifdef IPV6
	case AF_INET6:
	{
		return memcmp(&((struct sockaddr_in6 *) sa1)->sin6_addr,
			      &((struct sockaddr_in6 *) sa2)->sin6_addr,
			      sizeof(struct in6_addr));
	}
#endif

#ifdef AF_UNIX
	case AF_UNIX:
	{
		return strcmp(((struct sockaddr_un *) sa1)->sun_path,
			      ((struct sockaddr_un *) sa2)->sun_path);
	}
#endif
	}
	return -1;
}


/*
  sock_cmp_port.c
*/
int sock_cmp_port(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
	if (sa1->sa_family != sa2->sa_family) {
		return -1;
	}

	switch (sa1->sa_family) {
	case AF_INET:
	{
		return( ((struct sockaddr_in *) sa1)->sin_port ==
			((struct sockaddr_in *) sa2)->sin_port);
	}

#ifdef IPV6
	case AF_INET6:
	{
		return( ((struct sockaddr_in6 *) sa1)->sin6_port ==
			((struct sockaddr_in6 *) sa2)->sin6_port);
	}
#endif

	}
	return -1;
}



/*
  sock_get_port.c
*/
int sock_get_port(const struct sockaddr *sa, socklen_t salen)
{
	switch (sa->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		return sin->sin_port;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		return sin6->sin6_port;
	}
#endif
	}

	return -1;
}



/*
  sock_set_addr.c
*/
void sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr)
{
	switch (sa->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		memcpy(&sin->sin_addr, addr, sizeof(struct in_addr));
		return;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		memcpy(&sin6->sin6_addr, addr, sizeof(struct in6_addr));
		return;
	}
#endif
	}
}


/*
  sock_set_port.c
*/
void sock_set_port(struct sockaddr *sa, socklen_t salen, int port)
{
	switch (sa->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		sin->sin_port = port;
		return;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		sin6->sin6_port = port;
		return;
	}
#endif
	}
}



/*
  sock_set_wild.c
*/
void sock_set_wild(struct sockaddr *sa, socklen_t salen)
{
	const void *wildptr;

	switch (sa->sa_family) {
	case AF_INET:
	{
		static struct in_addr in4addr_any;
		in4addr_any.s_addr = htonl(INADDR_ANY);
		wildptr = &in4addr_any;
		break;
	}

#ifdef IPV6
	case AF_INET6:
	{
		wildptr = &in6addr_any; // in6addr_any.c
		break;
	}
#endif

	default:
		return;
	}
	sock_set_addr(sa, salen, wildptr);
	return;
}


/*
  sock_ntop
*/
char* sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char portstr[8];
	static char str[128];  // Unix domain is largest

	switch (sa->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		if (NULL == inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))) {
			return NULL;
		}
		if (0 != ntohs(sin->sin_port)) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return str;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		str[0] = '[';
		if (NULL == inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1)) {
			return NULL;
		}
		if (0 != ntohs(sin6->sin6_port)) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return str;
		}
		return (str + 1);
	}
#endif

#ifdef AF_UNIX
	case AF_UNIX:
	{
		struct sockaddr_un *unp = (struct sockaddr_un *) sa;

		/*
		   OK to have no pathname bound to the socket: happens on
		   every connect() unless client calls bind() first.
		*/
		if (0 == unp->sun_path[0]) {
			strcpy(str, "(no pathname bound)");
		} else {
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		}
		return str;
	}
#endif

	default:
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
		return str;
	}
	return NULL;
}


char* lothars__sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char *ptr = NULL;
	if (NULL == (ptr = sock_ntop(sa, salen))) {
		err_sys("sock_ntop error"); /* inet_ntop() sets errno */
	}
	return ptr;
}



/*
  sock_ntop_host.c
*/
char* sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	static char str[128];  // Unix domain is largest

	switch (sa->sa_family){
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		if (NULL == inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))) {
			return NULL;
		}
		return str;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		if (NULL == inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str))) {
			return NULL;
		}
		return str;
	}
#endif

#ifdef AF_UNIX
	case AF_UNIX:
	{
		struct sockaddr_un *unp = (struct sockaddr_un *) sa;
		/*
		   OK to have no pathname bound to the socket: happens on
		   every connect() unless client calls bind() first.
		*/
		if (0 == unp->sun_path[0]) {
			strcpy(str, "(no pathname bound)");
		} else {
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		}
		return str;
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
		return str;
	}
	return NULL;
}


char* lothars__sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	char *ptr = NULL;
	if (NULL == (ptr = sock_ntop_host(sa, salen))) {
		err_sys("sock_ntop_host error"); // inet_ntop() sets errno
	}
	return ptr;
}


/*
  readable_timeo.c
*/
int readable_timeo(int fd, int sec)
{
	fd_set rset;
	struct timeval tv;

	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return select(fd+1, &rset, NULL, NULL, &tv);
	// 4> 0 if descriptor is readable
}


int lothars__readable_timeo(int fd, int sec)
{
	int  bytes;
	if (0 > (bytes = readable_timeo(fd, sec))) {
		err_sys("%(): readable_timeo error", __func__);
	}
	return bytes;
}
