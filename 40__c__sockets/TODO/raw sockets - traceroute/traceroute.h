// traceroute.h
/*
//*/

#ifndef __TRACEROUTE_H_
#define __TRACEROUTE_H_

#include "../lib_socket/lib_socket.h"

#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>

#ifdef IPV6
# include <netinet/ip6.h>
# include <netinet/icmp6.h>
#endif

#define BUFSIZE 1500

/*
  outgoing UDP data
//*/
struct rec{
  // sequence number
  u_short rec_seq;
  
  // TTL (time to live) packet left with
  u_short rec_ttl;
  
  // time packet left
  struct timeval rec_tv;
};

// globals
char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];

// number of data bytes following the ICMP header
int datalen;
char *host=NULL;

//  source and destination port
u_short s_port, d_port;

// own pid
pid_t pid; 

// increment for each sendto()
int gotalarm, n_sent, probe, n_probes

  // send on UDP sock, read on raw ICMP sock
  , fd_send, fd_recv
  
  , ttl, max_ttl
  , verbose;

// function prototypes
const char* icmpcode_v4(int);
const char* icmpcode_v6(int);
int recv_v4(int, struct timeval*);
int recv_v6(int, struct timeval*);
void sig_alrm(int);
void traceloop();
void tv_sub(struct timeval*, struct timeval*); // XXX check - already in lib?


struct protocol{
  const char *(*icmpcode) (int);
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

