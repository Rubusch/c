// ping.h
/*
  ping implementation using raw sockets and ICMP packets like 
  the common ping
//*/


#ifndef PING_INCLUDE_GUARD
#define PING_INCLUDE_GUARD


#include "../lib_socket/lib_socket.h"


#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>


// BUFSIZ is system defined! This is 'bufsizE' - try and check other sizes
#define BUFSIZE 1500

char sendbuf[BUFSIZE];

// number of data bytes following ICMP header
int32_t datalen;
char *host;
int32_t fd_sock;
int32_t verbose;

// add one for each sendto
int32_t n_sent;

// our PID
pid_t pid;

// functions
void init_v6();
void proc_v4(char*, ssize_t, struct msghdr*, struct timeval*);
void proc_v6(char*, ssize_t, struct msghdr*, struct timeval*);
void send_v4();
void send_v6();
void readloop();
void sig_alrm(int32_t);
void tv_sub(struct timeval*, struct timeval*);
uint16_t in_cksum(uint16_t*, int32_t);

struct protocol{
  void (*fproc) (char*, ssize_t, struct msghdr*, struct timeval*);
  void (*fsend) ();
  void (*finit) ();
  struct sockaddr *sa_send; // sockaddr{} for send, from getaddrinfo()
  struct sockaddr *sa_recv; // sockaddr{} for receiving
  socklen_t salen;
  int32_t icmp_proto;
} *proto;


#ifdef IPV6
# include <netinet/ip6.h>
# include <netinet/icmp6.h>
#endif


#endif // PING_INCLUDE_GUARD

