// icmpd.h
/*
//*/

#ifndef __ICMP_H
#define __ICMP_H

#include "../lib_socket/lib_socket.h"

// server's well-known pathname
#define ICMPD_PATH "/tmp/icmpd"

struct icmpd_err{
  // EHOSTUNREACH, EMSGSIZE, ECONNREFUSED
  int icmpd_errno;

  // actual ICMPv[46] type
  char icmpd_type;
  
  // actual ICMPv[46] code
  char icmpd_code;

  // length of sockaddr{} that follows
  socklen_t icmpd_len;
  
  // sockaddr_storage handles any size
  struct sockaddr_storage icmpd_dest;
};


struct client{
  // unix domain stream socket to client
  int fd_conn;

  // AF_INET or AF_INET6
  int family;
  
  // local port bound to client's UDP socket
  int l_port;

  // network byte ordered
}client[FD_SETSIZE];


// globals
int fd_v4, fd_v6, fd_listen, idx_max, fd_max, n_ready;
fd_set rset, allset;
struct sockaddr_un cliaddr;

// prototypes
int readable_conn(int32_t);
int readable_listen();
int readable_v4();
int readable_v6();

#endif
