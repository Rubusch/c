// server.c
/*
  UDP echo server using select() and tcp connection establishemt/listen

  demonstrates usage of select
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
  forwards
*/

// error handling
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// socket
Sigfunc* lothars__signal(int, Sigfunc*);
pid_t lothars__fork();
// lothars__sendto()    
// lothars__recvfrom()   
void lothars__setsockopt(int, int, int, const void *, socklen_t);
int lothars__accept(int, struct sockaddr *, socklen_t *);
void lothars__close(int);
// lothars__bind()    
// lothars__socket()    


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


/*
  fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
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


void lothars__setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (0 > setsockopt(fd, level, optname, optval, optlen)) {
		err_sys("setsockopt error");
	}
}


pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
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


void lothars__close(int fd)
{
	if (-1 == close(fd)) {
		err_sys("close error");
	}
}


/********************************************************************************************/
// worker implementation

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
  udp echo server implementation via tcp connection
TODO investigate, comment                   

  creates a listening tcp socket for connection establishment,
  then forks out udp connections for performance
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

	// create listening TCP socket
	fd_listen = lothars__socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	lothars__setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	lothars__bind(fd_listen, (struct sockaddr*) &servaddr, LISTENQ);

	// create UDP socket
	fd_udp = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	lothars__bind(fd_udp, (struct sockaddr*) &servaddr, sizeof(servaddr));

	lothars__signal(SIGCHLD, sig_child); // must call waitpid()

	// 1. zero fd_set - select()
	FD_ZERO(&rset);

	maxfdpl = max(fd_listen, fd_udp) + 1;
	while (1) {

		// 2. add descriptors to the fd_set - select()
		FD_SET(fd_listen, &rset);
		FD_SET(fd_udp, &rset);

		// 3. select() multiplexing
// TODO check for using lothars__select()                
		if (0 > (n_ready = select(maxfdpl, &rset, NULL, NULL, NULL))) {
			if (errno == EINTR) { // caught interrupt
				continue;
			} else {
				err_sys("select error");
			}
		}

		// 4. select case "fd_listen"
		if (FD_ISSET(fd_listen, &rset)) {
			len = sizeof(cliaddr);
			fd_conn = lothars__accept(fd_listen, (struct sockaddr*) &cliaddr, &len);

			if (0 == (childpid = lothars__fork())) {
				// child process
				lothars__close(fd_listen);
				str_echo(fd_conn);
				exit(EXIT_SUCCESS);
			}

			// parent process
			lothars__close(fd_conn);
		}

		// 4. select case "fd_udp"
		if (FD_ISSET(fd_udp, &rset)) {
			len = sizeof(cliaddr);
			num = lothars__recvfrom(fd_udp, msg, MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
			lothars__sendto(fd_udp, msg, num, 0, (struct sockaddr*) &cliaddr, len);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

