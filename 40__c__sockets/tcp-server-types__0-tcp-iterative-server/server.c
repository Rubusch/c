// server.c
/*
  TCP iteractive server (no process control, faster is impossible!)

  An iterative TCP server proceses each client's request completely
  before moving on to the next client. Iterative TCP serers are rare!
//*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> /* wait() */
#include <unistd.h> /* read(), close() */
#include <sys/types.h> /* getaddrinfo() */
#include <sys/socket.h> /* getaddrinfo(), bind() */
#include <netdb.h> /* getaddrinfo() */
#include <arpa/inet.h> /* htons(), htonl(),... */
#include <stdarg.h>
#include <time.h>
#include <errno.h>


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */
#define BUFFSIZE 8192 /* buffer size for reads and writes */

// TODO avoid this for readability        
#define SA struct sockaddr

/*
  Following could be derived from SOMAXCONN in <sys/socket.h>, but many
  kernels still #define it as 5, while actually supporting many more 
*/
#define LISTENQ  1024                // 2nd argument to listen()


/*
  forwards
*/

int lothars__socket(int, int, int);
void lothars__bind(int, const SA *, socklen_t);
void lothars__listen(int, int);
int lothars__accept(int, struct sockaddr *, socklen_t *);
void lothars__write(int, void *, size_t);
void lothars__close(int);
// TODO


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
	char buf[MAXLINE + 1];

	errno_save = errno; // value caller might want printed

#ifdef HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap); // safe
#else
	vsprintf(buf, fmt, ap); // not safe
#endif

	n_len = strlen(buf);
	if(errnoflag){
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");

	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);

	return;
}

// TODO


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
	exit(1);
}


void lothars__bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if(0 > bind(fd, sa, salen)){
		err_sys("bind error");
	}
}


void lothars__listen(int fd, int backlog)
{
//	char *ptr = NULL; // TODO rm

// TODO needed here?
//  // may override 2nd argument with environment variable (stevens)
//  if (NULL != (ptr = getenv("LISTENQ"))) {
//    backlog = atoi(ptr);
//  }

	if(0 > listen(fd, backlog)){
		err_sys("listen error");
	}
}


int lothars__accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int res;

again:
	if(0 > (res = accept(fd, sa, salenptr))){
#ifdef EPROTO
		if((errno == EPROTO) || (errno == ECONNABORTED)){
#else
		if(errno == ECONNABORTED){
#endif
			goto again;
		}else{
			err_sys("accept error");
		}
	}
	return(res);
}


int lothars__socket(int family, int type, int protocol)
{
	int res;
	if(0 > (res = socket(family, type, protocol))){
		err_sys("socket error");
	}
	return res;
}


void lothars__write(int fd, void *ptr, size_t nbytes)
{
	if(nbytes != write(fd, ptr, nbytes)){
		err_sys("write error");
	}
}


void lothars__close(int fd)
{
	if(-1 == close(fd)){
		err_sys("close error");
	}
}
// TODO


/*
  main

  TODO
*/
int main(int argc, char** argv)
{
	int fd_listen, fd_conn;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	time_t ticks;

	// set up listen socket
	fd_listen = lothars__socket(AF_INET, SOCK_STREAM, 0);

	// set up server address
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
// TODO magic number - fix this         
	servaddr.sin_port = htons(27976); // connect to port 27976

	// bind "listen socket" to the "server address"
	lothars__bind(fd_listen, (SA*) &servaddr, sizeof(servaddr));

	// listen on socket (queue length == LISTENQ == 1024)
	lothars__listen(fd_listen, LISTENQ);

	// server loop
	while(1){
		// accept (blocking!) - ...listen socket to connection socket
		fd_conn = lothars__accept(fd_listen, (SA*) NULL, NULL);
    
		// example action: send a string containing the system time
		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
		fprintf(stdout, "sending \"%s\"\n", buf);
		lothars__write(fd_conn, buf, strlen(buf));
    
		// close connection after sending
		lothars__close(fd_conn);
	}
}
