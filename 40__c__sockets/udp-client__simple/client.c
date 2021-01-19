// client.c
/*
  UDP echo client
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> /* va_start(), va_end(),... */
#include <arpa/inet.h> /* inet_pton(), inet_ntop() */
#include <errno.h>


// constants

#define MAXLINE  4096 /* max text line length */


// forwards

ssize_t lothars__recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
char* lothars__fgets(char *, int, FILE *);
void lothars__fputs(const char *, FILE *);
void lothars__inet_pton(int, const char*, void*);
int lothars__socket(int, int, int);


// utilities

/*
   Print message and return to caller Caller specifies "errnoflag"
*/
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save, n_len;
	char buf[MAXLINE + 1];

	errno_save = errno; // value caller might want printed

	vsnprintf(buf, MAXLINE, fmt, ap); // safe
//	vsprintf(buf, fmt, ap); // alternatively, if no vsnprintf() available, not safe

	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");
	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);
}


// functions

/*
   Fatal error related to system call Print message and terminate
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
   Fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
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


char* lothars__fgets(char *ptr, int n, FILE *stream)
{
	char *rptr = NULL;
	if ( (NULL == (rptr = fgets(ptr, n, stream))) && ferror(stream)) {
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


int lothars__socket(int family, int type, int protocol)
{
	int res;
	if(0 > (res = socket(family, type, protocol))){
		err_sys("socket error");
	}
	return res;
}


/*
  main()...
*/
int main(int argc, char** argv)
{
	int fd_sock;
	struct sockaddr_in servaddr;
	char serverip[16]; memset(serverip, '\0', sizeof(serverip));
	char port[16]; memset(port, '\0', sizeof(port));

	if (3 != argc) {
		fprintf(stderr, "usage: %s <serverip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(serverip, argv[1], sizeof(serverip));
	fprintf(stdout, "serverip: '%s'\n", serverip);

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// init server address structure
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));

	// connect the client to the server
	lothars__inet_pton(AF_INET, serverip, &servaddr.sin_addr);

	// get the socket descriptor
	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	// connect the socket to the command line console
	{
		int n_bytes;
		char sendline[MAXLINE], recvline[MAXLINE + 1];
		const struct sockaddr *pservaddr = (struct sockaddr *) &servaddr;

		while (lothars__fgets(sendline, MAXLINE, stdin) != NULL) {
			lothars__sendto(fd_sock, sendline, strlen(sendline), 0, pservaddr, sizeof(servaddr));
			n_bytes = lothars__recvfrom(fd_sock, recvline, MAXLINE, 0, NULL, NULL);
			recvline[n_bytes] = 0; // null terminate TODO check '\0'
			lothars__fputs(recvline, stdout);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

