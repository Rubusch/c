#ifndef DEVKIT_SNIPPET_SOCKET
#define DEVKIT_SNIPPET_SOCKET


/*

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/


/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h> /* ctime() */
#include <fcntl.h> /* fcntl(), F_GETFL */

#include <stropts.h> /* ioctl() */
#include <sys/poll.h> /* struct pollfd */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h> /* uname() */
#include <linux/un.h> /* struct sockaddr_un */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "lib_socket.h"
#include "lib_streams.h"
#include "lib_read_write.h"
#include "lib_unix.h"
#include "lib_error.h"


/* constants / macros */

/*
  Following could be derived from SOMAXCONN in <sys/socket.h>, but many
  kernels still #define it as 5, while actually supporting many more
*/
#define LISTENQ  1024 /* listen queue size for listen(), can be set in env */
#define MAXLINE  4096 /* max text line length */
#define MAXFD 64 /* max file descriptors */
#define BUFFSIZE 8192 /* buffer size for reads and writes */
// default file access permissions for new files

//#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
// default permissions for new directories


/* forwards */

int connect_nonb(int, const struct sockaddr *, socklen_t, int);
//int connect_timeo(int, const struct sockaddr *, socklen_t, int);
void lothars__connect_timeo(int, const struct sockaddr *, socklen_t, int);
int daemon_init(const char *, int);
void daemon_inetd(const char *, int);
void dg_cli(FILE*, int, const struct sockaddr *, socklen_t);
void dg_echo(int, struct sockaddr*, socklen_t);
int lothars__family_to_level(int);
int lothars__socket_to_family(int);
char* gf_time();
char** lothars__my_addrs(int*);
void worker__echo_serv(int);
void worker__echo_cli(FILE *, int);
void worker__echo_serv_str(int);
void worker__echo_serv_bin(int);
void worker__echo_cli_bin(FILE*, int);
int lothars__tcp_connect(const char*, const char*);
int lothars__tcp_listen(const char*, const char*, socklen_t*);
void tv_sub(struct timeval*, struct timeval*);
int lothars__udp_client(const char*, const char*, struct sockaddr**, socklen_t*);
int lothars__udp_connect(const char*, const char*);
int lothars__udp_server(const char*, const char*, socklen_t*);
int lothars__writable_timeo(int, int);
int lothars__sock_bind_wild(int, int);
char* lothars__sock_ntop(const struct sockaddr*, socklen_t);
char* lothars__sock_ntop_host(const struct sockaddr*, socklen_t);
int lothars__readable_timeo(int, int);
int sock_cmp_addr(const struct sockaddr*, const struct sockaddr*, socklen_t);
int sock_cmp_port(const struct sockaddr*, const struct sockaddr*, socklen_t);
int sock_get_port(const struct sockaddr*, socklen_t);
void sock_set_addr(struct sockaddr*, socklen_t, const void*);
void sock_set_port(struct sockaddr*, socklen_t, int);
void sock_set_wild(struct sockaddr*, socklen_t);


#endif /* DEVKIT_SNIPPET_SOCKET */
