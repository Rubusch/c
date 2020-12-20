// server.c
/*
  TCP preforked server, descriptor passing

  In the previous preforked examples, the process never cared which
  child received a client connection. The OS handled this detail,
  giving one of the children the first call to accept(), or giving one
  of the children the file lock or the mutex lock. The results also
  show that the OS that we are measuring does this in a fair,
  round-robin fashion.

  FIXME!                                                                              
*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* read(), write(), close() */
#include <fcntl.h> /* fcntl(), FD_SET(), FD_ZERO(), FD_ISSET(),... */
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

/*
   min/max macros are not defined in C!

   generally in a better implementation use a __typeof__ generalization

   NB: if you are writing a header file that must work when included
   in ISO C programs, write __typeof__ instead of typeof
*/
#define max(a,b) (a > b ? a : b)
#define min(a,b) (a < b ? a : b)

/*
   we need the newer CMSG_LEN() and CMSG_SPACE() macros, but few
   implementations support them today.  These two macros really need
   an ALIGN() macro, but each implementation does this differently
*/
#ifndef CMSG_LEN
# define CMSG_LEN(size)  (sizeof(struct cmsghdr) + (size))
#endif

#ifndef CMSG_SPACE
# define CMSG_SPACE(size) (sizeof(struct cmsghdr) + (size))
#endif


/*
  forwards
*/

void lothars__dup2(int, int);
void* lothars__malloc(size_t size);
Sigfunc* lothars__signal(int, Sigfunc*);
ssize_t lothars__readline(int, void *, size_t);
int lothars__select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
pid_t lothars__fork();
void lothars__socketpair(int, int, int, int *);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
void lothars__listen(int, int);
int lothars__tcp_listen(const char*, const char*, socklen_t*);
int lothars__accept(int, struct sockaddr *, socklen_t *);
ssize_t lothars__read_fd(int, void*, size_t, int *);
ssize_t lothars__write_fd(int, void*, size_t, int);
ssize_t lothars__read(int, void *, size_t);
void lothars__write(int, void *, size_t);
void lothars__close(int);

void err_sys(const char *, ...);
void err_quit(const char *, ...);


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

// TODO readline_fd_doit(    
static ssize_t readline_fd_doit(int fd, char *ptr)
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
// TODO rn readline_fd(       
ssize_t readline_fd(int fd, void *vptr, size_t maxlen)
{
	ssize_t cnt, rc;
	char chr, *ptr = NULL;

	ptr = vptr;
	for (cnt = 1; cnt < maxlen; ++cnt) {
		if (1 == (rc = readline_fd_doit(fd, &chr))) { // main approach in readline_fd_doit
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


ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t cnt;

//#ifdef HAVE_MSGHDR_MSG_CONTROL
	union{
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr = NULL;
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
//#else
//	int newfd;
//	msg.msg_accrights = (caddr_t) &newfd;
//	msg.msg_accrightslen = sizeof(int);
//#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if (0 >= (cnt = recvmsg(fd, &msg, 0))) {
		return cnt;
	}

//#ifdef HAVE_MSGHDR_MSG_CONTROL
	if ( (NULL != (cmptr = CMSG_FIRSTHDR(&msg))) && (cmptr->cmsg_len == CMSG_LEN(sizeof(int)))) {
		if (cmptr->cmsg_level != SOL_SOCKET) {
			err_quit("control level != SOL_SOCKET");
		}
		if (cmptr->cmsg_type != SCM_RIGHTS) {
			err_quit("control type != SCM_RIGHTS");
		}
		*recvfd = *((int *) CMSG_DATA(cmptr));

	} else {
		*recvfd = -1;  // descriptor was not passed
	}
//#else
//	if (msg.msg_accrightslen == sizeof(int)) {
//		*recvfd = newfd;
//	} else {
//		*recvfd = -1;  // descriptor was not passed
//	}
//#endif

	return cnt;
}


ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	struct msghdr msg;
	struct iovec iov[1];

// TODO check out and simplify                                 
//#ifdef HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr cm;
		char    control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
//#else
//	msg.msg_accrights = (caddr_t) &sendfd;
//	msg.msg_accrightslen = sizeof(int);
//#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return sendmsg(fd, &msg, 0);
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


/*
  NB: dup2 implicitely allocates memory!
*/
void lothars__dup2(int fd1, int fd2)
{
	if (0 > dup2(fd1, fd2)) {
		err_sys("dup2 error");
	}
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
	if (0 > (bytes = readline_fd(fd, ptr, maxlen))) {
		err_sys("readline error");
	}
	return bytes;
}


int lothars__select(int nfds
		    , fd_set *readfds
		    , fd_set *writefds
		    , fd_set *exceptfds
		    , struct timeval *timeout)
{
	int  res;
	if (0 > (res = select(nfds, readfds, writefds, exceptfds, timeout))) {
		err_sys("select error");
	}
	return res;  // can return 0 on timeout
}


pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
}


void lothars__socketpair(int family, int type, int protocol, int *fd)
{
	if (0 > socketpair(family, type, protocol, fd)) {
		err_sys("socketpair error");
	}
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
	return res;
}


ssize_t lothars__read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	ssize_t cnt;
	if (0 > (cnt = read_fd(fd, ptr, nbytes, recvfd))) {
		err_sys("read_fd error");
	}
	return cnt;
}


ssize_t lothars__write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	ssize_t  bytes;

	if (0 > (bytes = write_fd(fd, ptr, nbytes, sendfd))) {
		err_sys("write_fd error");
	}

	return(bytes);
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


ssize_t lothars__read(int fd, void *ptr, size_t nbytes)
{
	ssize_t  bytes;

	if (-1 == (bytes = read(fd, ptr, nbytes))) {
		err_sys("read error");
	}
	return(bytes);
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


typedef struct{
	pid_t child_pid;   // process ID
	int32_t fd_child_pipe; // parent's stream pipe to / from child
	int32_t child_status;  // 0 = ready
	long child_count;  // number of connections handled
} Child_t;
Child_t* child_ptr;


/*
  child - routine

  do anything, read, write, etc.. some action
*/
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
  child - main function
*/
void child_main(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
	char chr;
	int32_t fd_conn;
	ssize_t num;

	fprintf(stdout, "child %d: starting\n", getpid());

	while (1) {
		if (0 == (num = lothars__read_fd(STDERR_FILENO, &chr, 1, &fd_conn))) {
			err_quit("read_fd() returned 0");
		}

		if (0 > fd_conn) {
			err_quit("no descriptor from read_fd()");
		}

		// process request
		child_routine(fd_conn);
		lothars__close(fd_conn);

		// tell parent we're ready again
		lothars__write(STDERR_FILENO, "", 1);
	}
}


/*
  child - creator
*/
pid_t child_make(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
	int32_t fd_sock[2];
	pid_t pid;

	lothars__socketpair(AF_LOCAL, SOCK_STREAM, 0, fd_sock);

	if (0 < (pid = lothars__fork())) {
		// close other end of pipe
		lothars__close(fd_sock[1]);

		// init Child_t struct to be passed via the pipe
		child_ptr[idx].child_pid = pid;
		child_ptr[idx].fd_child_pipe = fd_sock[0];
		child_ptr[idx].child_status = 0;

		// parent returns
		return pid;
	}

	// child's stream pipe to parent
	lothars__dup2(fd_sock[idx], STDERR_FILENO);

	// close pipe
	lothars__close(fd_sock[0]);
	lothars__close(fd_sock[1]);

	// child does not need this open
	lothars__close(fd_listen);

	// never returns!!
	child_main(idx, fd_listen, addrlen);

	return pid;
}


/*
  print user time and system time on shutting down
*/
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
*/
void sig_int(int32_t signo)
{
	pr_cpu_time();
	if(NULL != child_ptr){
		free(child_ptr);
		child_ptr = NULL;
	}
	exit(EXIT_SUCCESS);
}


/********************************************************************************************/


/*
  main


*/
int main(int argc, char** argv)
{
	int32_t fd_listen, n_avail, fd_max, n_sel, fd_conn, rc;
	ssize_t num;
	fd_set set_r, set_master;
	socklen_t addrlen, clilen;
	struct sockaddr* cliaddr = NULL;
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

	// init select
	FD_ZERO(&set_master);
	FD_SET(fd_listen, &set_master);

	// TODO
	fd_max = fd_listen;

	cliaddr = lothars__malloc(addrlen);
	n_avail = NCHILDREN;
	child_ptr = lothars__malloc(NCHILDREN * sizeof(*child_ptr));
  
	// prefork all the children
	int32_t idx;
	for (idx=0; idx<NCHILDREN; ++idx) {
		child_make(idx, fd_listen, addrlen); // only parent returns
		FD_SET(child_ptr[idx].fd_child_pipe, &set_master);
		fd_max = max(fd_max, child_ptr[idx].fd_child_pipe);
	}

	// signal handler, set up only once in parent for children
	lothars__signal(SIGINT, sig_int);

	while (1) {
		set_r = set_master;
		if (0 >= n_avail) {
			// turn off if no available children
			FD_CLR(fd_listen, &set_r);
		}

		// the select I/O multiplexing
		n_sel = lothars__select(fd_max+1, &set_r, NULL, NULL, NULL);

		// check for new connections
		if (FD_ISSET(fd_listen, &set_r)) {
			clilen = addrlen;
			fd_conn = lothars__accept(fd_listen, cliaddr, &clilen);

			// get next available child index
			for (idx=0; idx<NCHILDREN; ++idx) {
				if (child_ptr[idx].child_status == 0) {
					break; // available
				}
			}

			// exit, in case index ran throu..
			if (idx == NCHILDREN) {
				err_quit("no available children");
			}

			child_ptr[idx].child_status = 1; // mark child as busy
			++child_ptr[idx].child_count;
			--n_avail;

			// TODO
			num = lothars__write_fd(child_ptr[idx].fd_child_pipe, "", 1, fd_conn);

			// TODO
			lothars__close(fd_conn);

			// TODO
			if (--n_sel == 0) {
				continue; // all done with select() results
			}
		}

		// find any newly-available children
		for (idx=0; idx<NCHILDREN; ++idx) {
			if (FD_ISSET(child_ptr[idx].fd_child_pipe, &set_r)) {
				if (0 == (num = lothars__read(child_ptr[idx].fd_child_pipe, &rc, 1))) {
					err_quit("child %d terminated unexepctedly", idx);
				}
				child_ptr[idx].child_status = 0;
				++n_avail;
				if (--n_sel == 0) {
					break; // all done with select() results
				}
			}
		}
	}

	exit(EXIT_SUCCESS);
}

