// client.c
/*
  tcp server types - TCP testing client for all server types
*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> /* wait() */
#include <unistd.h> /* read(), write(), fork(), close() */
#include <netdb.h> /* socket(), SOCK_STREAM, AF_UNSPEC, getaddrinfo() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <arpa/inet.h> /* inet_pton(), inet_ntop(),.. */
#include <errno.h>

/*
  constants
*/

#define MAXLINE 4096 /* max text line length */

/*
  forwards
*/

void err_sys(const char *, ...);
void err_quit(const char *fmt, ...);

void lothars__connect(int, const struct sockaddr *, socklen_t);
int lothars__socket(int, int, int);
char *lothars__fgets(char *, int, FILE *);
void lothars__fputs(const char *, FILE *);
void lothars__inet_pton(int, const char *, void *);
ssize_t lothars__readline_fd(int, void *, size_t);
void lothars__writen(int, void *, size_t);

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
	vsnprintf(buf, MAXLINE, fmt, ap);
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

void lothars__connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > connect(fd, sa, salen)) {
		err_sys("connect error");
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

char *lothars__fgets(char *ptr, int n, FILE *stream)
{
	char *rptr = NULL;
	if ((NULL == (rptr = fgets(ptr, n, stream))) && ferror(stream)) {
		err_sys("fgets error");
	}
	return rptr;
}

void lothars__fputs(const char *ptr, FILE *stream)
{
	if (EOF == fputs(ptr, stream)) {
		err_sys("fputs error");
	}
}

void lothars__inet_pton(int family, const char *strptr, void *addrptr)
{
	int res;
	if (0 > (res = inet_pton(family, strptr, addrptr))) {
		err_sys("inet_pton error for %s", strptr); // errno set
	} else if (0 == res) {
		err_quit("inet_pton error for %s", strptr); // errno not set
	}
}

/*
  readline_fd

  readline via file descriptor, while the lib readline() does not take
  a filedescriptor
*/
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t readline_fd_read(int fd, char *ptr)
{
	if (0 >= read_cnt) {
	again:
		if (0 > (read_cnt = read(fd, read_buf, sizeof(read_buf)))) {
			if (errno == EINTR) {
				goto again;
			}
			return -1;
		} else if (read_cnt == 0) {
			return 0;
		}
		read_ptr = read_buf;
	}

	--read_cnt;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t readline_fd(int fd, void *vptr, size_t maxlen)
{
	ssize_t cnt, rc;
	char chr, *ptr;

	ptr = vptr;
	for (cnt = 1; cnt < maxlen; ++cnt) {
		if (1 == (rc = readline_fd_read(fd, &chr))) {
			*ptr++ = chr;
			if (chr == '\n') {
				break; // newline is stored, like fgets()
			}

		} else if (rc == 0) {
			*ptr = 0;
			return (cnt - 1); // EOF, n - 1 bytes were read

		} else {
			return -1; // error, errno set by read()
		}
	}

	*ptr = 0; /* null terminate like fgets() */
	return cnt;
}

ssize_t lothars__readline_fd(int fd, void *ptr, size_t maxlen)
{
	ssize_t bytes;
	if (0 > (bytes = readline_fd(fd, ptr, maxlen))) {
		err_sys("readline error");
	}
	return bytes;
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

/********************************************************************************************/
// worker implementation

void worker__echo_cli(FILE *fp, int fd_sock)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	memset(sendline, '\0', sizeof(sendline));
	memset(recvline, '\0', sizeof(recvline));
	while (NULL != lothars__fgets(sendline, sizeof(sendline), fp)) {
		lothars__writen(fd_sock, sendline, strlen(sendline));
		if (0 ==
		    lothars__readline_fd(fd_sock, recvline, sizeof(recvline)))
			err_quit("%s(): server terminated prematurely\n",
				 __func__);

		lothars__fputs(recvline, stdout);
	}
}

/********************************************************************************************/

/*
  main()

  simple single tcp client

  a tcp server is expected, serverip and port needs to be provided
*/
int main(int argc, char **argv)
{
	int fd_sock;
	struct sockaddr_in servaddr;
	char serverip[16];
	memset(serverip, '\0', sizeof(serverip));
	char port[16];
	memset(port, '\0', sizeof(port));

	if (3 != argc) {
		fprintf(stderr, "usage: %s <serverip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(serverip, argv[1], sizeof(serverip));
	fprintf(stdout, "serverip: '%s'\n", serverip);

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	fd_sock = lothars__socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));
	lothars__inet_pton(AF_INET, serverip, &servaddr.sin_addr);
	lothars__connect(fd_sock, (struct sockaddr *)&servaddr,
			 sizeof(servaddr));

	worker__echo_cli(stdin, fd_sock);

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
