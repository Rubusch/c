/*
  TCP preforked server, pthread locking around accept

  (with thread mutex locking to protect accept)
*/

#define _XOPEN_SOURCE 600

#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* read(), write(), close() */
#include <netdb.h> /* freeadddrinfo(), getaddrinfo() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <sys/wait.h> /* waitpid(), SIGINT,... */
#include <sys/resource.h> /* getrusage(), struct rusage,... */
#include <time.h> /* time(), ctime() */
#include <pthread.h> /* pthread_mutex_lock(), pthread_mutex_unlock(),... */
#include <fcntl.h> /* open() */
#include <sys/mman.h> /* mmap() */
#include <errno.h>

/*
  constants
*/

#define MAXLINE 4096 /* max text line length */
#define LISTENQ 1024 /* the listen queue - serving as backlog for listen */
#define MAXN 1234 /*  max number of bytes to request from server */
#define NWORKER 7 /* number of pre-allocated threads */

typedef void Sigfunc(int); /* convenience: for signal handlers */

/*
  forwards
*/

// error handling
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// commons
void *lothars__malloc(size_t);
int lothars__open(const char *, int, mode_t);
void *lothars__mmap(void *, size_t, int, int, int, off_t);

// pthreads
void lothars__pthread_mutexattr_init(pthread_mutexattr_t *);
void lothars__pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
void lothars__pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
void lothars__pthread_mutex_lock(pthread_mutex_t *);
void lothars__pthread_mutex_unlock(pthread_mutex_t *);

// socket
Sigfunc *lothars__signal(int, Sigfunc *);
ssize_t lothars__readline(int, void *, size_t);
pid_t lothars__fork();
void lothars__setsockopt(int, int, int, const void *, socklen_t);
void lothars__listen(int, int);
int lothars__tcp_listen(const char *, const char *, socklen_t *);
int lothars__accept(int, struct sockaddr *, socklen_t *);
void lothars__write(int, void *, size_t);
void lothars__close(int *);

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
	memset(buf, '\0', sizeof(buf));

	errno_save = errno; // value caller might want printed
	vsnprintf(buf, MAXLINE, fmt, ap); // safe
	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s",
			 strerror(errno_save));
	}

	strcat(buf, "\n");

	fflush(stdout); // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);

	return;
}

// used "private" (.c file static) variables for the limited readline implementation
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t readline_fd_doit(int fd, char *ptr)
{
	if (0 >= read_cnt) {
	again:
		if (0 >
		    (read_cnt = read(
			     fd, read_buf,
			     sizeof(read_buf)))) { // use read() to construct a readline() with size limits
			if (errno == EINTR)
				goto again; // only in case of interrrupted, try again
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
ssize_t readline_fd(int fd, void *vptr, size_t maxlen)
{
	ssize_t cnt, rc;
	char chr, *ptr = NULL;

	ptr = vptr;
	for (cnt = 1; cnt < maxlen; ++cnt) {
		if (1 ==
		    (rc = readline_fd_doit(
			     fd, &chr))) { // main approach in readline_fd_doit
			*ptr++ = chr;
			if (chr == '\n')
				break; // newline is stored, like fgets()

		} else if (rc == 0) {
			*ptr = 0;
			return (cnt - 1); // EOF, n - 1 bytes were read

		} else {
			return -1; // error, errno set by read()
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
	va_list ap;
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
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void *lothars__malloc(size_t size)
{
	void *ptr = NULL;
	if (NULL == (ptr = malloc(size))) {
		err_sys("malloc error");
	}
	return ptr;
}

int lothars__open(const char *pathname, int oflag, mode_t mode)
{
	int fd;
	if (-1 == (fd = open(pathname, oflag, mode))) {
		err_sys("open error for %s", pathname);
	}
	return fd;
}

/*
  mmap() - memory map wrapper

  checking the return object of mmap() against a void* typedef'd equivalent to '-1'
  i.e. mmap() was fail
*/
void *lothars__mmap(void *addr, size_t len, int prot, int flags, int fd,
		    off_t offset)
{
	void *ptr = NULL;
	if (((void *)-1) == (ptr = mmap(addr, len, prot, flags, fd, offset))) {
		err_sys("mmap error");
	}
	return ptr;
}

void lothars__pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	int res;
	if (0 == (res = pthread_mutexattr_init(attr))) {
		return;
	}
	errno = res;
	err_sys("pthread_mutexattr_init error");
}

void lothars__pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int flag)
{
	int res;
	if (0 == (res = pthread_mutexattr_setpshared(attr, flag))) {
		return;
	}
	errno = res;
	err_sys("pthread_mutexattr_setpshared error");
}

void lothars__pthread_mutex_init(pthread_mutex_t *mptr,
				 pthread_mutexattr_t *attr)
{
	int res;
	if (0 == (res = pthread_mutex_init(mptr, attr))) {
		return;
	}
	errno = res;
	err_sys("pthread_mutex_init error");
}

void lothars__pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int res;
	if (0 == (res = pthread_mutex_lock(mptr))) {
		return;
	}
	errno = res;
	err_sys("pthread_mutex_lock error");
}

void lothars__pthread_mutex_unlock(pthread_mutex_t *mptr)
{
	int res;
	if (0 == (res = pthread_mutex_unlock(mptr))) {
		return;
	}
	errno = res;
	err_sys("pthread_mutex_unlock error");
}

Sigfunc *lothars__signal(int signo, Sigfunc *func) // for our signal() function
{
	Sigfunc *sigfunc = NULL;
	if (SIG_ERR == (sigfunc = signal(signo, func))) {
		err_sys("signal error");
	}
	return sigfunc;
}

ssize_t lothars__readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t bytes;
	if (0 > (bytes = readline_fd(fd, ptr, maxlen))) {
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

void lothars__setsockopt(int fd, int level, int optname, const void *optval,
			 socklen_t optlen)
{
	if (0 > setsockopt(fd, level, optname, optval, optlen)) {
		err_sys("setsockopt error");
	}
}

void lothars__listen(int fd, int backlog)
{
	if (0 > listen(fd, backlog)) {
		err_sys("listen error");
	}
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
	const int on = 1;
	struct addrinfo hints, *res = NULL, *ressave = NULL;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("tcp_listen error for %s, %s: %s", host, serv,
			 gai_strerror(eai));
	}

	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype,
				  res->ai_protocol);
		if (0 > listenfd) {
			continue; // error, try next one
		}
		lothars__setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on,
				    sizeof(on));
		// set socket no linger and reuse addresse to avoid zombie sockets lying around in Linux

		if (0 ==
		    bind(listenfd, res->ai_addr,
			 res->ai_addrlen)) { // try to bind to the client (tcp)
			break; // success
		}
		lothars__close(&listenfd); // bind error, close and try next one

	} while (
		NULL !=
		(res = res->ai_next)); // iterate over all clients obtained in address info (ai)

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

int lothars__accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int res;
again:
	if (0 > (res = accept(fd, sa, salenptr))) {
		if ((errno == EPROTO) ||
		    (errno ==
		     ECONNABORTED)) { // deal with some POSIX.1 errors...
			goto again;
		} else {
			err_sys("accept error");
		}
	}
	return res;
}

void lothars__write(int fd, void *ptr, size_t nbytes)
{
	if (nbytes != write(fd, ptr, nbytes)) {
		err_sys("write error");
	}
}

/*
  The close() function shall deallocate the file descriptor indicated
  by fd. To deallocate means to make the file descriptor available for
  return by subsequent calls to open() or other functions that
  allocate file descriptors. All outstanding record locks owned by the
  process on the file associated with the file descriptor shall be
  removed (that is, unlocked).

  If close() is interrupted by a signal that is to be caught, it shall
  return -1 with errno set to [EINTR] and the state of fildes is
  unspecified. If an I/O error occurred while reading from or writing
  to the file system during close(), it may return -1 with errno set
  to [EIO]; if this error is returned, the state of fildes is
  unspecified.

  This wrapper sets the fp to NULL;

  #include <unistd.h>

  @fd: Points to the file descriptor to the specific connection.
*/
void lothars__close(int *fd)
{
	if (NULL == fd) {
		fprintf(stderr, "%s() fd was NULL\n", __func__);
		return;
	}
	if (-1 == close(*fd)) {
		err_sys("%s() error", __func__);
	}
	*fd = 0;
#if _XOPEN_SOURCE >= 500
	sync();
#endif /* _XOPEN_SOURCE */
}

/********************************************************************************************/
// worker implementation

static pid_t *pid_children;

void lock_init(char *);
void lock_wait();
void lock_release();

/*
  worker - routine

  do anything, read, write, etc.. some action
*/
void worker_routine(int32_t fd_sock)
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
			fprintf(stdout,
				"child %d: read '%s', n_read '%ld' - reading over, return\n",
				getpid(), line, n_read);
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
  worker - main function
*/
void worker_main(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
	int32_t fd_conn;
	socklen_t clilen;
	struct sockaddr cliaddr[addrlen];

	fprintf(stdout, "child %d: starting\n", getpid());

	while (1) {
		clilen = addrlen;

		// mutex lock
		lock_wait();

		// blocks...
		fd_conn = lothars__accept(fd_listen, cliaddr, &clilen);

		// mutex unlock
		lock_release();

		worker_routine(fd_conn);

		lothars__close(&fd_conn);
		fprintf(stdout, "child %d: READY.\n", getpid());
	}
}

/*
  worker - constructor
*/
pid_t worker_make(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
	pid_t pid;

	if (0 == (pid = lothars__fork())) {
		// child
		worker_main(idx, fd_listen, addrlen); // never returns
	}

	// parent
	return pid;
}

// mutex pointer as "private" (global static in .c)
static pthread_mutex_t *mx_ptr;

/*
  init pthread mutexes
*/
void lock_init(char *pathname)
{
	pthread_mutexattr_t mxattr;

	int32_t fd = lothars__open("/dev/zero", O_RDWR, 0);
	mx_ptr = lothars__mmap(0, sizeof(pthread_mutex_t),
			       PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	lothars__close(&fd);

	lothars__pthread_mutexattr_init(&mxattr);
	lothars__pthread_mutexattr_setpshared(&mxattr, PTHREAD_PROCESS_SHARED);
	lothars__pthread_mutex_init(mx_ptr, &mxattr);
}

/*
  lock blocking
*/
void lock_wait()
{
	lothars__pthread_mutex_lock(mx_ptr);
}

/*
  release lock
*/
void lock_release()
{
	lothars__pthread_mutex_unlock(mx_ptr);
}

/*
  print user time and system time on shutting down
*/
void pr_cpu_time()
{
	double user, sys;
	struct rusage usage_parent, usage_child;

	// init parent ressource usage
	if (0 > getrusage(RUSAGE_SELF, &usage_parent)) {
		err_sys("getrusage(parent) error");
	}

	// init child ressource usage
	if (0 > getrusage(RUSAGE_CHILDREN, &usage_child)) {
		err_sys("getrusage(child) error");
	}

	// calculate user time
	user = (double)usage_parent.ru_utime.tv_sec +
	       usage_parent.ru_utime.tv_usec / 1000000.0;
	user += (double)usage_child.ru_utime.tv_sec +
		usage_child.ru_utime.tv_usec / 1000000.0;

	// calculate system time
	sys = (double)usage_parent.ru_stime.tv_sec +
	      usage_parent.ru_stime.tv_usec / 1000000.0;
	sys += (double)usage_child.ru_stime.tv_sec +
	       usage_child.ru_stime.tv_usec / 1000000.0;

	fprintf(stdout, "\nuser time = %gs, sys time = %gs\n", user, sys);
}

/*
  int sig handler - action when server is shutdown by CTRL + c
*/
void sig_int(int32_t signo)
{
	int32_t idx;

	for (idx = 0; idx < NWORKER; ++idx) {
		kill(pid_children[idx], SIGTERM);
	}
	free(pid_children);
	pid_children = NULL;

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
int main(int argc, char **argv)
{
	int32_t fd_listen, idx;
	socklen_t addrlen;
	char port[16];
	memset(port, '\0', sizeof(port));

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
	pid_children = lothars__malloc(NWORKER * sizeof(*pid_children));

	// init file lock
	lock_init("/tmp/lock.1234567"); // one lock file for all children

	for (idx = 0; idx < NWORKER; ++idx) {
		// create children, only parent returns
		pid_children[idx] = worker_make(idx, fd_listen, addrlen);
	}

	// signal handler, set up only once in parent for children
	lothars__signal(SIGINT, sig_int);

	while (1) {
		pause();
	}

	exit(EXIT_SUCCESS);
}
