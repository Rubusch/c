// server.c
/*
  TCP and UDP echo server

  combined concurrent TCP echo server with iterative UDP echo server
  using select() to multiplex a TCP and UDP socket
*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* read(), write(), close() */
#include <netdb.h> /* freeadddrinfo(), getaddrinfo() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <sys/wait.h> /* waitpid(), SIGINT,... */
#include <sys/resource.h> /* getrusage(), struct rusage,... */
#include <time.h> /* time(), ctime() */
#include <errno.h>


/*
  constants
*/

#define MAXLINE 4096 /* max text line length */
#define LISTENQ 1024 /* the listen queue - serving as backlog for listen */

typedef void Sigfunc(int); /* convenience: for signal handlers */

/*
  min / max - improve using __typeof__
*/
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))


/*
  forwards
*/

// error handling
void err_sys(const char *, ...);

// socket
Sigfunc* lothars__signal(int, Sigfunc*);
void lothars__writen(int, void *, size_t);
int lothars__accept(int, struct sockaddr *, socklen_t *);
void lothars__bind(int, const struct sockaddr *, socklen_t);
void lothars__listen(int, int);
ssize_t lothars__recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
int lothars__socket(int, int, int);
pid_t lothars__fork();
void lothars__close(int);


/*
  internal helpers
*/

/*
  print message and return to caller Caller specifies "errnoflag"

  error handling taken from "Unix Sockets" (Stevens)
*/
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save, n_len;
	char buf[MAXLINE + 1]; memset(buf, '\0', sizeof(buf));

	errno_save = errno; // value caller might want printed
	vsnprintf(buf, MAXLINE, fmt, ap); // safe
	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");

	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);

	return;
}


/*
  helpers / wrappers

  mainly to cover error handling and display error message
*/

/*
  fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}



Sigfunc* lothars__signal(int signo, Sigfunc *func) // for our signal() function
{
	Sigfunc *sigfunc = NULL;
	if(SIG_ERR == (sigfunc = signal(signo, func))){
		err_sys("signal error");
	}
	return sigfunc;
}


/*
  writen

  write "num" bytes to a descriptor
*/
ssize_t writen(int fd, const void *vptr, size_t num)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = num;
	while (0 < nleft) {
		if (0 >= (nwritten = write(fd, ptr, nleft))) {
			if ((nwritten < 0) && (errno == EINTR)) {
				// and call write() again
				nwritten = 0;
			} else {
				// error
				return -1;
			}
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return num;
}


void lothars__writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes) {
		err_sys("writen error");
	}
}


int lothars__accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int res;
again:
	if (0 > (res = accept(fd, sa, salenptr))) {
		if ((errno == EPROTO) || (errno == ECONNABORTED)) { // deal with some POSIX.1 errors...
			goto again;
		} else {
			err_sys("accept error");
		}
	}
	return res;
}


void lothars__bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > bind(fd, sa, salen)) {
		err_sys("bind error");
	}
}


void lothars__listen(int fd, int backlog)
{
	char *ptr = NULL;
	// can override 2nd argument with environment variable
	if (NULL != (ptr = getenv("LISTENQ"))) {
		backlog = atoi(ptr);
	}

	if (0 > listen(fd, backlog)) {
		err_sys("listen error");
	}
}


ssize_t lothars__recvfrom(int fd
		  , void *ptr
		  , size_t nbytes
		  , int flags
		  , struct sockaddr *sa
		  , socklen_t *salenptr)
{
	ssize_t  res;
	if (0 > (res = recvfrom(fd, ptr, nbytes, flags, sa, salenptr))) {
		err_sys("recvfrom error");
	}
	return res;
}


void lothars__sendto( int fd
	      , const void *ptr
	      , size_t nbytes
	      , int flags
	      , const struct sockaddr *sa
	      , socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t) nbytes) {
		err_sys("sendto error");
	}
}


void lothars__setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (0 > setsockopt(fd, level, optname, optval, optlen)) {
		err_sys("setsockopt error");
	}
}


int lothars__socket(int family, int type, int protocol)
{
	int res;
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("socket error");
	}
	return res;
}


pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
}


void lothars__close(int fd)
{
	if (-1 == close(fd)) {
		err_sys("close error");
	}
}


/********************************************************************************************/
// worker implementation

void worker_main(int fd_sock)
{
	ssize_t res;
	char buf[MAXLINE];
	memset(buf, '\0', sizeof(buf));

// FIXME: tcp echoing buggy, check out and fix bug                 
again:
	while (0 < (res = read(fd_sock, buf, MAXLINE))) {
		fprintf(stdout, "tcp connected - received and echoing: '%s'\n", buf);
		lothars__writen(fd_sock, buf, res);
	}

	if ((0 > res) && (errno == EINTR)) {
		goto again;
	} else if (0 > res) {
		err_sys("%s(): read error", __func__);
	}
}


/*
  mini signal handler, calls waitpid to avoid zombies
*/
void sig_child(int signo)
{
	pid_t pid;
	int status;

	while (0 < (pid = waitpid(-1, &status, WNOHANG))) {
		printf("child %d terminated\n", pid);
	}
}


/********************************************************************************************/


/*
  main - tcp and udp echo server implementation
*/
int main(int argc, char** argv)
{
	int fd_listen, fd_conn, fd_udp, n_ready, maxfdpl;
	char msg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t num;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	char port[16]; memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(port, argv[1], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// socket - create listening TCP socket!
	fd_listen = lothars__socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(port));

	lothars__setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// bind port (tcp)
	lothars__bind(fd_listen, (struct sockaddr*) &servaddr, sizeof(servaddr));

	// listen (tcp)
	lothars__listen(fd_listen, LISTENQ);

	// create UDP socket
	fd_udp = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(port));

	lothars__bind(fd_udp, (struct sockaddr*) &servaddr, sizeof(servaddr));

	lothars__signal(SIGCHLD, sig_child); // must call waitpid()

	// 1. zero fd_set - select()
	FD_ZERO(&rset);

	// which one is more recent (higher fd number), i.e. is there a 'connected' UDP client to serve?
	maxfdpl = max(fd_listen, fd_udp) + 1;

	while (1) {
		// reset
		memset(msg, '\0', sizeof(msg));

		// 2. add descriptors to the fd_set - select()
		FD_SET(fd_listen, &rset);
		FD_SET(fd_udp, &rset);

		// 3. select() multiplexing
		if (0 > (n_ready = select(maxfdpl, &rset, NULL, NULL, NULL))) {
			if (errno == EINTR) { // caught interrupt
				// NB: if this is not desired, take lothars__select()
				continue;
			} else {
				err_sys("select error");
			}
		}

		// 4. select case "fd_listen"
		if (FD_ISSET(fd_listen, &rset)) {
			len = sizeof(cliaddr);

			// accept (tcp)
			fd_conn = lothars__accept(fd_listen, (struct sockaddr*) &cliaddr, &len);

			if (0 == (childpid = lothars__fork())) {
				// child process (tcp)
				lothars__close(fd_listen);
				worker_main(fd_conn);
				exit(EXIT_SUCCESS);
			}

			// parent process
			lothars__close(fd_conn);
		}

		// 4. select case "fd_udp"
		if (FD_ISSET(fd_udp, &rset)) {
			len = sizeof(cliaddr);
			num = lothars__recvfrom(fd_udp, msg, MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
			fprintf(stdout, "udp connected - received and echoing: '%s'\n", msg);
			lothars__sendto(fd_udp, msg, num, 0, (struct sockaddr*) &cliaddr, len);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

