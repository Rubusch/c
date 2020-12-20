// server.c
/*
  TCP preforked server, no locking around accept

  (each child calling accept)
//*/

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
#define MAXN 1234 /*  max number of bytes to request from server */
#define NCHILDREN 7 /* number of pre-allocated threads */

typedef void Sigfunc(int); /* convenience: for signal handlers */

static pid_t *pid_children;


/*
  forwards
*/
Sigfunc* lothars__signal(int, Sigfunc*);
ssize_t lothars__readline(int, void *, size_t);
pid_t lothars__fork();
void lothars__listen(int, int);
int lothars__accept(int, struct sockaddr *, socklen_t *);
int lothars__tcp_listen(const char*, const char*, socklen_t*);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
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


// used "private" (.c file static) variables for the limited readline implementation
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t fd_read(int fd, char *ptr)
{
	if (0 >= read_cnt) {
	again:
		if (0 > (read_cnt = read(fd, read_buf, sizeof(read_buf)))) { // use read() to construct a readline() with size limits
			if (errno == EINTR) goto again; // only in case of interrrupted, try again
			return -1; // else return as error code
		} else if (read_cnt == 0) {
			return 0;
		}
		read_ptr = read_buf;
	}

	--read_cnt;
	*ptr = *read_ptr++;
	return 1;
}


/*
  a pimped readline() version, reading from an filedescriptor (fd),
  into anything (vptr), respecting a maxlen and dealing with some
  erros, implementation is based on read()
*/
ssize_t fd_readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t cnt, rc;
	char chr, *ptr = NULL;

	ptr = vptr;
	for (cnt = 1; cnt < maxlen; ++cnt) {
		if (1 == (rc = fd_read(fd, &chr))) { // main approach in fd_read
			*ptr++ = chr;
			if (chr == '\n')
				break; // newline is stored, like fgets()

		} else if (rc == 0) {
			*ptr = 0;
			return (cnt - 1); // EOF, n - 1 bytes were read

		} else {
			return -1;  // error, errno set by read()
		}
	}

	*ptr = 0; // null terminate like fgets()
	return cnt;
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


void* lothars__malloc(size_t size)
{
	void *ptr = NULL;
	if (NULL == (ptr = malloc(size))) {
		err_sys("malloc error");
	}
	return ptr;
}


Sigfunc* lothars__signal(int signo, Sigfunc *func) // for our signal() function
{
	Sigfunc *sigfunc = NULL;
	if(SIG_ERR == (sigfunc = signal(signo, func))){
		err_sys("signal error");
	}
	return sigfunc;
}


ssize_t lothars__readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t bytes;
	if (0 > (bytes = fd_readline(fd, ptr, maxlen))) {
		err_sys("readline error");
	}
	return bytes;
}


pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
}


void lothars__listen(int fd, int backlog)
{
	if (0 > listen(fd, backlog)) {
		err_sys("listen error");
	}
}


int lothars__accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int res;

again:
	if (0 > (res = accept(fd, sa, salenptr))) {
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


/*
  tcp_listen() similar to W. Richard Stevens' implementation

  We place the wrapper function here, not in wraplib.c, because some
  XTI programs need to include wraplib.c, and it also defines
  a Tcp_listen() function. (stevens)
*/
int lothars__tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int listenfd, eai;
	const int  on = 1;
	struct addrinfo hints, *res = NULL, *ressave = NULL;

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
		// set socket no linger and reuse addresse to avoid zombie sockets lying around in Linux

		if (0 == bind(listenfd, res->ai_addr, res->ai_addrlen)) { // try to bind to the client (tcp)
			break; // success
		}
		lothars__close(listenfd); // bind error, close and try next one

	} while (NULL != (res = res->ai_next)); // iterate over all clients obtained in address info (ai)

	if (NULL == res) { // errno from final socket() or bind()
		err_sys("tcp_listen error for %s, %s", host, serv);
	}

	lothars__listen(listenfd, LISTENQ);

	if (addrlenp) {
		*addrlenp = res->ai_addrlen; // return size of protocol address
	}

	freeaddrinfo(ressave); // free the gai stuff needed for listen

	return listenfd; // returned bound fd
}


void lothars__setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (0 > setsockopt(fd, level, optname, optval, optlen)) {
		err_sys("setsockopt error");
	}
}


void lothars__write(int fd, void *ptr, size_t nbytes)
{
	if (nbytes != write(fd, ptr, nbytes)) {
		err_sys("write error");
	}
}


void lothars__close(int fd)
{
	if (-1 == close(fd)) {
		err_sys("close error");
	}
}




/********************************************************************************************/
// child implementation

/*
  child sub, called by child main (do some read, and echo write)
//*/
void child_routine(int32_t fd_sock)
{
	int32_t towrite;
	ssize_t n_read;
	char line[MAXLINE], result[MAXN];
	time_t ticks;

	memset(line, '\0', sizeof(line));
	memset(result, '\0', sizeof(result));

	// server loop
	while (1) {
		// read
		if (0 == (n_read = lothars__readline(fd_sock, line, MAXLINE))) {
			// nothing more to read -> exit
			fprintf(stdout, "child %d: read '%s', n_read '%ld' - reading over, return\n", getpid(), line, n_read);
			return;
		}

		// get as number (long)
		towrite = atol(line);

		// check if valid
		if ((0 >= towrite) || (MAXN < towrite)) {
			err_quit("client request for %d bytes", towrite);
		}
		// if ok, write and return the received message back to socket

		// example action: send a string containing the system time
		ticks = time(NULL);
		snprintf(result, sizeof(result), "%.24s", ctime(&ticks));
		fprintf(stdout, "child %d: sending \"%s\"\n", getpid(), result);
		lothars__write(fd_sock, result, towrite);
	}
}


/*
  child main
*/
void child_main(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
	int32_t fd_conn;
	socklen_t clilen;
	struct sockaddr cliaddr[addrlen];

	fprintf(stdout, "child %d: starting\n", getpid());

	while (1) {

		/*
		  WARNING: accept can cause "collisions", this here is only
		  the template for the following "preforked" implementations,
		  therefore here no locking at all - blocking by accept!

		  In a real scenario there should be at least a select() before
		  accept()
		*/

		clilen = addrlen;

		// blocks...
		fd_conn = lothars__accept(fd_listen, cliaddr, &clilen);

		child_routine(fd_conn);

		lothars__close(fd_conn);
		fprintf(stdout, "child %d: READY.\n", getpid());
	}
}


/*
  fork
*/
pid_t child_make(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
	pid_t pid;

	if (0 == (pid = lothars__fork())) {
		// child
		child_main(idx, fd_listen, addrlen);
		// never returns
	}

	// parent
	return pid;
}

/*
  example code: print system times when shutting down by CTRL + c
//*/
void pr_cpu_time()
{
	double user, sys;
	struct rusage usage_parent, usage_child;
	if (0 > getrusage(RUSAGE_SELF, &usage_parent)) {
		err_sys("getrusage(parent) error");
	}

	if (0 > getrusage(RUSAGE_CHILDREN, &usage_child)) {
		err_sys("getrusage(child) error");
	}

	user = (double) usage_parent.ru_utime.tv_sec + usage_parent.ru_utime.tv_usec / 1000000.0;
	user += (double) usage_child.ru_utime.tv_sec + usage_child.ru_utime.tv_usec / 1000000.0;

	sys = (double) usage_parent.ru_stime.tv_sec + usage_parent.ru_stime.tv_usec / 1000000.0;
	sys += (double) usage_child.ru_stime.tv_sec + usage_child.ru_stime.tv_usec / 1000000.0;

	fprintf(stdout, "\nuser time = %gs, sys time = %gs\n", user, sys);
}


/*
  final action when server is shutdown by CTRL + c
//*/
void sig_int(int32_t signo)
{
	int32_t idx;

	for (idx=0; idx<NCHILDREN; ++idx) {
		kill(pid_children[idx], SIGTERM);
	}
	free(pid_children); pid_children = NULL;

	while (0 < wait(NULL))
		;

	if (errno != ECHILD) {
		err_sys("wait() error");
	}

	pr_cpu_time();
	exit(EXIT_SUCCESS);
}


/********************************************************************************************/


/*
  main

  the pre-allocating server implementation
*/
int main(int32_t argc, char** argv)
{
	int32_t fd_listen, idx;
	socklen_t addrlen;
	char port[16]; memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(port, argv[1], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// set socket listen on provided port
	fd_listen = lothars__tcp_listen(NULL, port, &addrlen);
	// or alternatively, listen on host ip 10.0.2.15 and port
	// (in case provide ip via argv)
	//fd_listen = lothars__tcp_listen("10.0.2.15", port, &addrlen);

	// set number of children
	pid_children = lothars__malloc(NCHILDREN * sizeof(*pid_children));
	for (idx=0; idx<NCHILDREN; ++idx) {
		// create children, only parent returns
		pid_children[idx] = child_make(idx, fd_listen, addrlen);
	}

	// signal handler, set up only once in parent for children
	lothars__signal(SIGINT, sig_int);

	while (1) {
		pause();
	}

	exit(EXIT_SUCCESS);
}
