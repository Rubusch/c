// server.c
/*
  TCP concurrent server, one child per client

  slowest TCP server type
//*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* read(), write(), close() */
#include <arpa/inet.h> /* htons(), htonl(), accept(), socket(),... */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <time.h> /* time(), ctime() */
#include <errno.h>


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */
#define BUFFSIZE 8192 /* buffer size for reads and writes */
#define LISTENQ 1024 /* the listen queue - serving as backlog for listen */
#define MAXN 1234 /*  max number of bytes to request from server */


/*
  forwards
*/
//_malloc()
//_signal()
pid_t lothars__fork();
void lothars__write(int, void *, size_t);
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


/********************************************************************************************/
// example code - code for some demo actions, not necessarily related to the server type


/*
  example code: do anything, read, write, etc.. some action
//*/
void child_routine(int fd_sock)
{
	int32_t towrite;
	ssize_t n_read;
	char line[MAXLINE], result[MAXN];

	// server loop
	while(1){
		// read
		if(0 == (n_read = _readline(fd_sock, line, MAXLINE))){
			// nothing more to read -> exit
			fprintf(stdout, "reading over, %d returns\n", getpid());
			return;
		}

		// get as long's
		towrite = atol(line);

		// check if valid
		if((0 >= towrite) || (MAXN < towrite)){
			err_quit("client request for %d bytes", towrite);
		}

		// if ok, write and return the received message back to socket
		lothars__write(fd_sock, result, towrite);
	}
}


/*
  child signal handler - calls waitpid()

  called when a child process has terminated the work
//*/
void sig_chld(int signo){
	pid_t pid;
	int32_t status;

	while(0 < (pid = waitpid(-1, &status, WNOHANG))){
		printf("child %d terminated\n", pid);
	}
}


/*
  example code: print system time and usage at CTRL + C termination of server
//*/
void pr_cpu_time()
{
	double user, sys;
	struct rusage usage_parent, usage_child;

	// init parent ressource usage
	if(0 > getrusage(RUSAGE_SELF, &usage_parent)){
		err_sys("getrusage(parent) error");
	}

	// init child ressource usage
	if(0 > getrusage(RUSAGE_CHILDREN, &usage_child)){
		err_sys("getrusage(child) error");
	}

	// calculate user time
	user = (double) usage_parent.ru_utime.tv_sec + usage_parent.ru_utime.tv_usec / 1000000.0;
	user += (double) usage_child.ru_utime.tv_sec + usage_child.ru_utime.tv_usec / 1000000.0;

	// calculate system time
	sys = (double) usage_parent.ru_stime.tv_sec + usage_parent.ru_stime.tv_usec / 1000000.0;
	sys += (double) usage_child.ru_stime.tv_sec + usage_child.ru_stime.tv_usec / 1000000.0;

	// print message at termination of server
	printf("\nuser time = %gs, sys time = %gs\n", user, sys);
}


/*
  final sig handler - action when server is shutdown by CTRL + c
//*/
void sig_int(int signo)
{
	pr_cpu_time();
	exit(EXIT_SUCCESS);
}

/********************************************************************************************/

/*
  main

  the concurrent server implementation
*/
int main(int argc, char** argv)
{
	int32_t fd_listen, fd_conn;
	pid_t child_pid;
	socklen_t clilen, addrlen;
	struct sockaddr *cliaddr=NULL;
	cliaddr = _malloc(addrlen);
	char port[16]; memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(port, argv[1], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// set socket listen on port 27976
	fd_listen = _tcp_listen(NULL, port, &addrlen);
	// or alternatively, listen on host ip 10.0.2.15 and port 27976
	//fd_listen = _tcp_listen("10.0.2.15", port, &addrlen);

	// set signal handlers
	_signal(SIGCHLD, sig_chld);
	_signal(SIGINT, sig_int);

	while(1){
		clilen = addrlen;
		if(0 > (fd_conn = accept(fd_listen, cliaddr, &clilen))){
			if(errno == EINTR){
				continue;
			}
			// or
			err_sys("accept() error");
		}

		if(0 == (child_pid = lothars__fork())){
			// child process

			// close listening socket
			lothars__close(fd_listen);

			// process loop - never returnes
			child_routine(fd_conn);

			exit(EXIT_SUCCESS);
		}

		// parent process

		// closes connected socket
		lothars__close(fd_conn);
		fprintf(stdout, "READY.\n");
		if(NULL != cliaddr) free(cliaddr); cliaddr = NULL;
	}

	// never reaches here
	exit(EXIT_SUCCESS);
}

