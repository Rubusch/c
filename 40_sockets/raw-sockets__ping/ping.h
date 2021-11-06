// ping.h
/*
  ping implementation using raw sockets and ICMP packets like the
  common ping
*/

#ifndef PING_INCLUDE_GUARD
#define PING_INCLUDE_GUARD

#define _XOPEN_SOURCE 600
#define _GNU_SOURCE /* sync() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> /* va_start(), va_end(),... */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> /* gettimeofday() */
#include <sys/wait.h> /* wait() */
#include <sys/un.h>
#include <unistd.h> /* read(), write(), fork(), close() */
#include <netdb.h> /* socket(), SOCK_STREAM, AF_UNSPEC, getaddrinfo() */
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#ifdef IPV6
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#endif

#include <errno.h>

/*
  constants
*/

#define MAXLINE 4096 /* max text line length */
typedef void Sigfunc(int); /* give signal handlers a type instead of void* */

// BUFSIZ is system defined! This is 'bufsizE' - try and check other sizes
#define BUFSIZE 1500

char sendbuf[BUFSIZE];

// number of data bytes following ICMP header
int32_t datalen;
char *host;
int32_t fd_sock;
int32_t verbose = 0;

// add one for each sendto
int32_t n_sent;

// our PID
pid_t pid;

/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// socket
int lothars__socket(int, int, int);
struct addrinfo *lothars__host_serv(const char *, const char *, int, int);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *,
		     socklen_t);

// unix
Sigfunc *lothars__signal_intr(int, Sigfunc *);
void *lothars__malloc(size_t);
void lothars__gettimeofday(struct timeval *);

// inet
char *lothars__sock_ntop_host(const struct sockaddr *, socklen_t);

// snippet socket
void tv_sub(struct timeval *, struct timeval *);

// internal
void proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
void send_v4();

void init_v6();
void proc_v6(char *, ssize_t, struct msghdr *, struct timeval *);
void send_v6();

void readloop();
void sig_alrm(int32_t);

uint16_t checksum(uint16_t *, int32_t);

/*
  The approach is to use an instance of "protocol" for IPv4 and
  IPv6. Protocol offers processing, sending and optional init.
*/
struct protocol {
	void (*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
	void (*fsend)();
	void (*finit)();
	struct sockaddr *sa_send; // sockaddr{} for send, from getaddrinfo()
	struct sockaddr *sa_recv; // sockaddr{} for receiving
	socklen_t salen;
	int32_t icmp_proto;
} * proto;

#endif
