/*
  tcp server types

  prethreaded, main thread calls accept (blocking)

  fastest server, handling several threads / processes

FIXME: this server approach may work, but may hang up not reproducibly
(needs investigation)
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
#include <errno.h>

/*
  constants
*/

#define MAXLINE 4096 /* max text line length */
#define LISTENQ 1024 /* the listen queue - serving as backlog for listen */
#define MAXNCLI 32 /* max number of threads running */
#define NWORKER 7
#define MAXN 1234 /*  max number of bytes to request from server */

typedef void Sigfunc(int); /* convenience: for signal handlers */

/*
  forwards
*/

// error handling
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// commons
void *lothars__malloc(size_t);

// pthreads
void lothars__pthread_create(pthread_t *, const pthread_attr_t *,
			     void *(*)(void *), void *);
void lothars__pthread_mutex_lock(pthread_mutex_t *);
void lothars__pthread_mutex_unlock(pthread_mutex_t *);
void lothars__pthread_cond_signal(pthread_cond_t *);
void lothars__pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);

// socket
Sigfunc *lothars__signal(int, Sigfunc *);
ssize_t lothars__readline(int, void *, size_t);
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

void lothars__pthread_create(pthread_t *tid, const pthread_attr_t *attr,
			     void *(*func)(void *), void *arg)
{
	int res;
	if (0 == (res = pthread_create(tid, attr, func, arg))) {
		return;
	}
	errno = res;
	err_sys("pthread_create error");
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

void lothars__pthread_cond_signal(pthread_cond_t *cptr)
{
	if (0 != (errno = pthread_cond_signal(cptr))) {
		err_sys("pthread_cond_signal error");
	}
}

void lothars__pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr)
{
	if (0 != (errno = pthread_cond_wait(cptr, mptr))) {
		err_sys("pthread_cond_wait error");
	}
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

typedef struct {
	pthread_t thread_tid; // thread id
	long thread_count; // number of connections handled
} Thread_t;

Thread_t *thread_ptr;

// array of socket descriptors
int fd_cli[MAXNCLI];

// thread index
int idx_thread;

// main thread index (only one main thread!)
int idx_mainthread;

pthread_mutex_t mutex_fd_cli = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_fd_cli = PTHREAD_COND_INITIALIZER;

// forward declarations to avoid warnings
void worker_make(int);
void *worker_main(void *);

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
void *worker_main(void *arg)
{
	int fd_conn;
	int32_t idx = *(int *)arg;
	fprintf(stdout, "thread %d starting\n", idx);

	while (1) {
		lothars__pthread_mutex_lock(&mutex_fd_cli);

		// check if thread index comes to main thread index -> wait
		while (idx_thread == idx_mainthread) {
			lothars__pthread_cond_wait(&cond_fd_cli, &mutex_fd_cli);
		}

		// connected socket to service
		fd_conn = fd_cli[idx_thread];

		// increment idx_thread, go to next thread
		if (++idx_thread == MAXNCLI)
			idx_thread = 0;
		lothars__pthread_mutex_unlock(&mutex_fd_cli);
		++thread_ptr[idx].thread_count;

		// process request
		worker_routine(fd_conn);

		lothars__close(&fd_conn);
		fprintf(stdout, "child %d: READY.\n", getpid());
	}
}

/*
  worker - constructor

  only main thread returns
*/
void worker_make(int idx)
{
	// NB: try to avoid passing stack variables to pthread_create(),
	// prefer heap variables the alternative approach is to signal
	// via conditional wait/signal mechanism, as shown here
	lothars__pthread_create(&thread_ptr[idx].thread_tid, NULL, &worker_main,
				(void *)&idx);
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
	pr_cpu_time();
	exit(EXIT_SUCCESS);
}

/********************************************************************************************/

/*
  main

*/
int main(int argc, char **argv)
{
	int32_t fd_listen;
	int32_t fd_conn;
	socklen_t clilen, addrlen;
	struct sockaddr *cliaddr = NULL;
	char port[16];
	memset(port, '\0', sizeof(port));
	int32_t idx;

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

	cliaddr = lothars__malloc(addrlen);

	// init thread pointer, number of threads = 7
	thread_ptr = lothars__malloc(NWORKER * sizeof(*thread_ptr));
	idx_thread = idx_mainthread = 0;

	// create threadpool with NWORKER threads
	for (idx = 0; idx < NWORKER; ++idx) {
		worker_make(idx); // only main thread returns
	}

	// wake thread in case thread index (idx_thread) was main thread index
	// (idx_mainthread), and therefore thread was waiting under conditional

	lothars__signal(SIGINT, sig_int);

	while (1) {
		clilen = addrlen;

		fd_conn = lothars__accept(fd_listen, cliaddr, &clilen);

		lothars__pthread_mutex_lock(&mutex_fd_cli);
		fd_cli[idx_mainthread] = fd_conn;

		// increment index idx_mainthread, in case reset to 0
		if (++idx_mainthread == MAXNCLI)
			idx_mainthread = 0;

		// check if main thread index (idx_mainthread) is thread index (idx_thread)
		if (idx_mainthread == idx_thread) {
			err_quit("idx_mainthread = idx_thread = %d",
				 idx_mainthread);
		}

		// start threads via conditional signaling
		lothars__pthread_cond_signal(&cond_fd_cli);
		lothars__pthread_mutex_unlock(&mutex_fd_cli);
	}

	exit(EXIT_SUCCESS);
}
