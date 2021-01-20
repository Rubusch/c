#ifndef __TRACEROUTE_H_
#define __TRACEROUTE_H_


#define _GNU_SOURCE /* sync(), ICMP.. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> /* va_start(), va_end(),... */

#include <sys/time.h> /* gettimeofday() */
#include <sys/wait.h> /* wait() */
#include <sys/un.h> /* struct sockaddr_un */
#include <unistd.h> /* read(), write(), fork(), close() */
#include <netdb.h> /* socket(), SOCK_STREAM, AF_UNSPEC, getaddrinfo() */
#include <arpa/inet.h> /* inet_ntop() */

#include <netinet/ip_icmp.h> /* struct ip, ICMP_* */
#include <netinet/udp.h> /* struct udphdr */
#ifdef IPV6
# include <netinet/ip6.h>
# include <netinet/icmp6.h>
#endif

#include <errno.h>

/*
  constants
*/

#define BUFSIZE 1500 /* buffer size for packet frames */
#define MAXLINE 4096 /* max text line length */

// function prototypes
const char* icmpcode_v4(int);
const char* icmpcode_v6(int);
int recv_v4(int, struct timeval*);
int recv_v6(int, struct timeval*);
void sig_alrm(int);
void worker_routine(void);
void tv_sub(struct timeval*, struct timeval*); // XXX check - already in lib?


/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// socket
int lothars__socket(int, int, int);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
void lothars__bind(int, const struct sockaddr *, socklen_t);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
struct addrinfo* lothars__host_serv(const char*, const char*, int, int);

// unix
typedef void Sigfunc(int); /* give signal handlers a type instead of void* */
Sigfunc* lothars__signal_intr(int, Sigfunc*);  // missing
void lothars__gettimeofday(struct timeval *);
void* lothars__malloc(size_t);

// inet
char* lothars__sock_ntop_host(const struct sockaddr*, socklen_t);

// snippets sockeet
void tv_sub(struct timeval*, struct timeval*);
void sock_set_port(struct sockaddr*, socklen_t, int);
int sock_cmp_addr(const struct sockaddr*, const struct sockaddr*, socklen_t);


/*
  outgoing UDP data
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);


struct rec {
	// sequence number
	u_short rec_seq;

	// TTL (time to live) packet left with
	u_short rec_ttl;

	// time packet left
	struct timeval rec_tv;
};


struct protocol {
	// print the ICMP code, depending IPv4 or IPv6
	const char *(*icmpcode) (int);

	// receiver function, to be called after sendto() sent out a probe
	int (*recv) (int, struct timeval*);

	// sockaddr{} for send, from getaddrinfo
	struct sockaddr *sa_send;

	// sockaddr{} for receiving
	struct sockaddr *sa_recv;

	// last sockaddr{} for receiving
	struct sockaddr *sa_last;

	// sockaddr{} for binding source port
	struct sockaddr *sa_bind;

	// length of sockaddr{}-es
	socklen_t sa_len;

	// IPPROTO_xxx vlaue for ICMP
	int icmp_proto;

	// setsockopt() level to set TTL
	int ttl_level;

	// setsockopt() name to set TTL
	int ttl_optname;
} *proto;


#endif

