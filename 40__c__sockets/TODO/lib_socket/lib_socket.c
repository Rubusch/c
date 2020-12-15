// wrap_socket_lib.c
/*
  contains
  - connect_nonb.c
  - connect_timeo.c
  - daemon_init.c
  - daemon_inetd.c
  - dg_cli.c
  - dg_echo.c
  - family_to_level.c
  - gf_time.c
  - host_serv.c
  - my_addrs.c
  - readable_timeo.c
  - readline.c
  - readn.c
  - read_fd.c
  - signal_intr.c
  - sock_bind_wild.c
  - sock_cmp_addr.c
  - sock_cmp_port.c
  - sock_get_port.c
  - sock_set_addr.c
  - sock_set_wild.c
  - sock_ntop.c
  - sock_ntop_host.c
  - sockfd_to_family.c
  - str_echo.c
  - str_cli.c
  - tcp_connect.c
  - tcp_listen.c
  - tv_sub.c
  - udp_client.c
  - udp_connect.c
  - udp_server.c
  - writable_timeo.c
  - writen.c
  - write_fd.c

  - signal.c
  - sockatmark.c
//*/

#include "lib_socket.h"

#include <syslog.h> // daemon_init.c, daemon_inetd.c
#include <time.h> // gf_time.c
#include <sys/utsname.h> // my_addrs.c

#ifdef HAVE_SOCKADDR_DL_STRUCT
# include <net/if_dl.h>
#endif



/*
  connect_nonb.c
*/
int connect_nonb(int sockfd, const SA *saptr, socklen_t salen, int nsec)
{
  int flags, res, error;
  socklen_t len;
  fd_set rset, wset;
  struct timeval tval;
  
  flags = _fcntl(sockfd, F_GETFL, 0);
  _fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

  error = 0;
  if(0 > (res = connect(sockfd, saptr, salen))){
    if(errno != EINPROGRESS){
      return(-1);
    }
  } 
 
  // Do whatever we want while the connect is taking place.
  
  if(0 == res){
    goto done; // connect completed immediately 
  }
  
  FD_ZERO(&rset);
  FD_SET(sockfd, &rset);
  wset = rset;
  tval.tv_sec = nsec;
  tval.tv_usec = 0;
  
  if(0 == (res = _select(sockfd+1, &rset, &wset, NULL, (nsec ? &tval : NULL)))){
    close(sockfd);  // timeout 
    errno = ETIMEDOUT;
    return(-1);
  }
  
  if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)){
    len = sizeof(error);

    if(0 > getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len)){
      return(-1); // Solaris pending error
    }
  }else{
    err_quit("select error: sockfd not set");
  }
  
 done:
  _fcntl(sockfd, F_SETFL, flags); // restore file status flags 
  
  if(error){
    close(sockfd); // just in case 
    errno = error;
    return(-1);
  }
  return(0);
}



/*
  connect_timeo.c
*/
static void connect_alarm(int);

int connect_timeo(int sockfd, const SA *saptr, socklen_t salen, int nsec)
{
  Sigfunc *sigfunc;
  int res;
  
  sigfunc = _signal(SIGALRM, connect_alarm);
  if(alarm(nsec) != 0){
    err_msg("connect_timeo: alarm was already set");
  }
  
  if(0 > (res = connect(sockfd, saptr, salen))){
    close(sockfd);
    if(errno == EINTR){
      errno = ETIMEDOUT;
    }
  }

  alarm(0); // turn off the alarm 
  _signal(SIGALRM, sigfunc); // restore previous signal handler
  
  return(res);
}



static void connect_alarm(int signo)
{
  return; // just interrupt the connect()
}



void _connect_timeo(int fd, const SA *sa, socklen_t salen, int sec)
{
  if(0 > connect_timeo(fd, sa, salen, sec)){
    err_sys("connect_timeo error");
  }
}



/*
  daemon_init.c
*/
#define MAXFD 64


extern int daemon_proc; // defined in error.c 

int daemon_init(const char *pname, int facility)
{
  int  idx;
  pid_t pid;
  
  if(0 > (pid = _fork())){
    return (-1);
  }else if(pid){
    _exit(0); // parent terminates 
  }
  
  // child 1 continues... 

  if(0 > setsid()){ // become session leader 
    return (-1);
  }
  
  _signal(SIGHUP, SIG_IGN);
  if(0 > (pid = _fork())){
    return (-1);
  }else if(pid){
    _exit(0); // child 1 terminates 
  }

  // child 2 continues... 
  
  daemon_proc = 1; // for err_XXX() functions
  
  chdir("/"); // change working directory 
  
  // close off file descriptors
  for(idx = 0; idx < MAXFD; ++idx){
    close(idx);
  }
  
  // redirect stdin, stdout, and stderr to /dev/null
  open("/dev/null", O_RDONLY);
  open("/dev/null", O_RDWR);
  open("/dev/null", O_RDWR);
  
  openlog(pname, LOG_PID, facility);
  
  return (0); // success 
}



/*
  daemon_inetd.c
*/
extern int daemon_proc; // defined in error.c 


void daemon_inetd(const char *pname, int facility)
{
  daemon_proc = 1;  // for our err_XXX() functions 
  openlog(pname, LOG_PID, facility);
}



/*
  dg_cli.c
*/
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
  int n_bytes;
  char sendline[MAXLINE], recvline[MAXLINE + 1];
  
  while(_fgets(sendline, MAXLINE, fp) != NULL){    
    _sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
    
    n_bytes = _recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
    
    recvline[n_bytes] = 0; // null terminate 
    _fputs(recvline, stdout);
  }
}



/*
  dg_echo.c
*/
void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
  int n_bytes;
  socklen_t len;
  char mesg[MAXLINE];
  
  while(1){
    len = clilen;
    n_bytes = _recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
    
    _sendto(sockfd, mesg, n_bytes, 0, pcliaddr, len);
  }
}



/*
  family_to_level.c
*/
int family_to_level(int family)
{
  switch(family){
  case AF_INET:
    return IPPROTO_IP;

#ifdef IPV6
  case AF_INET6:
    return IPPROTO_IPV6;
#endif

  default:
    return -1;
  }
}



int _family_to_level(int family)
{
  int rc;
  
  if(0 > (rc = family_to_level(family))){
    err_sys("family_to_level error");
  }
  
  return rc;
}



/*
  gf_time.c
*/
char* gf_time(void)
{
  struct timeval tv;
  time_t time;
  static char  str[30];
  char *ptr;
  
  if(0 > gettimeofday(&tv, NULL)){
    err_sys("gettimeofday error");
  }
  
  time = tv.tv_sec; // POSIX says tv.tv_sec is time_t; some BSDs don't agree.
  ptr = ctime(&time);
  strcpy(str, &ptr[11]);
  // Fri Sep 13 00:00:00 1986\n\0
  // 0123456789012345678901234 5 
  snprintf(str+8, sizeof(str)-8, ".%06ld", tv.tv_usec);
  
  return(str);
}




/* 
   host_serv.c
*/
struct addrinfo* host_serv(const char *host, const char *serv, int family, int socktype)
{
  struct addrinfo hints, *res;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME; // always return canonical name
  hints.ai_family = family;  // AF_UNSPEC, AF_INET, AF_INET6, etc. 
  hints.ai_socktype = socktype; // 0, SOCK_STREAM, SOCK_DGRAM, etc.
  
  if(0 != getaddrinfo(host, serv, &hints, &res)){
    return(NULL);
  }
  
  return(res); // return pointer to first on linked list 
}


/*
  There is no easy way to pass back the integer return code from
  getaddrinfo() in the function above, short of adding another argument
  that is a pointer, so the easiest way to provide the wrapper function
  is just to duplicate the simple function as we do here.
*/
struct addrinfo* _host_serv(const char *host, const char *serv, int family, int socktype)
{
  struct addrinfo hints, *res;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME; // always return canonical name 
  hints.ai_family = family;  // 0, AF_INET, AF_INET6, etc. 
  hints.ai_socktype = socktype; // 0, SOCK_STREAM, SOCK_DGRAM, etc. 

  int eai;
  if(0 != (eai = getaddrinfo(host, serv, &hints, &res))){
    err_quit("_host_serv error for %s, %s: %s"
             , (host == NULL) ? "(no hostname)" : host
             , (serv == NULL) ? "(no service name)" : serv
             , gai_strerror(eai));
  }
  
  return(res); // return pointer to first on linked list 
}



/*
  my_addrs.c
*/
char** my_addrs(int *addrtype)
{
  struct hostent *hptr = NULL;
  struct utsname myname;
  
  if(0 > uname(&myname)){
    return NULL;
  }
  
  if(NULL == (hptr = gethostbyname(myname.nodename))){
    return NULL;
  }
  
  *addrtype = hptr->h_addrtype;

  return hptr->h_addr_list;
}



char** _my_addrs(int *pfamily)
{
  char **pptr;
  if(NULL == (pptr = my_addrs(pfamily))){
    err_sys("my_addrs error");
  }

  return pptr;
}



/*
  readable_timeo.c
*/
int readable_timeo(int fd, int sec)
{
  fd_set rset;
  struct timeval tv;
  
  FD_ZERO(&rset);
  FD_SET(fd, &rset);
  
  tv.tv_sec = sec;
  tv.tv_usec = 0;
  
  return select(fd+1, &rset, NULL, NULL, &tv);
  // 4> 0 if descriptor is readable
}



int _readable_timeo(int fd, int sec)
{
  int  bytes;  
  if(0 > (bytes = readable_timeo(fd, sec))){
    err_sys("readable_timeo error");
  }
  return(bytes);
}



/*
  readline.c
*/
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];


static ssize_t my_read(int fd, char *ptr)
{
  if(0 >= read_cnt){
  again:
    if(0 > (read_cnt = read(fd, read_buf, sizeof(read_buf)))){
      if(errno == EINTR){
        goto again;
      }
      return -1;
    }else if(read_cnt == 0){
      return(0);
    }
    read_ptr = read_buf;
  }
  
  --read_cnt;
  *ptr = *read_ptr++;
  return 1;
}



ssize_t readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t cnt, rc;
  char chr, *ptr;
  
  ptr = vptr;
  for(cnt = 1; cnt < maxlen; ++cnt){
    if(1 == (rc = my_read(fd, &chr))){
      *ptr++ = chr;
      if(chr == '\n'){
        break; // newline is stored, like fgets()
      }
 
    }else if(rc == 0){
      *ptr = 0;
      return (cnt - 1); // EOF, n - 1 bytes were read
    
    }else{
      return -1;  // error, errno set by read()
    }
  }
  
  *ptr = 0; /* null terminate like fgets() */
  return cnt;
}



ssize_t readlinebuf(void **vptrptr)
{
  if(read_cnt){
    *vptrptr = read_ptr;
  }
  return read_cnt;
}



ssize_t _readline(int fd, void *ptr, size_t maxlen)
{
  ssize_t bytes;  
  if(0 > (bytes = readline(fd, ptr, maxlen))){
    err_sys("readline error");
  }
  return(bytes);
}



/*
  readn.c

  Read "num" bytes from a descriptor. 
*/
ssize_t readn(int fd, void *vptr, size_t num)
{
  size_t nleft;
  ssize_t nread;
  char *ptr=NULL;
  
  ptr = vptr;
  nleft = num;
  while(nleft > 0){
    if( (nread = read(fd, ptr, nleft)) < 0){
      if(errno == EINTR){
        nread = 0;  // and call read() again
      }else{
        return -1;
      }
    }else if(nread == 0){
      break; // EOF
    }   
    nleft -= nread;
    ptr   += nread;
  }
  return (num - nleft);  // return >= 0
}


ssize_t _readn(int fd, void *ptr, size_t nbytes)
{
  ssize_t  res;
  if(0 > (res = readn(fd, ptr, nbytes))){
    err_sys("readn error");
  }
  return res;
}



/*
  read_fd.c
*/
ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
  struct msghdr msg;
  struct iovec iov[1];
  ssize_t cnt;
  
#ifdef HAVE_MSGHDR_MSG_CONTROL
  union{
    struct cmsghdr cm;
    char control[CMSG_SPACE(sizeof(int))];
  } control_un;
  struct cmsghdr *cmptr = NULL;
  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);
#else
  int newfd;  
  msg.msg_accrights = (caddr_t) &newfd;
  msg.msg_accrightslen = sizeof(int);
#endif
  
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  
  iov[0].iov_base = ptr;
  iov[0].iov_len = nbytes;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  
  if(0 >= (cnt = recvmsg(fd, &msg, 0))){
    return cnt;
  } 
 
#ifdef HAVE_MSGHDR_MSG_CONTROL
  if( (NULL != (cmptr = CMSG_FIRSTHDR(&msg))) && (cmptr->cmsg_len == CMSG_LEN(sizeof(int)))){
    if(cmptr->cmsg_level != SOL_SOCKET){
      err_quit("control level != SOL_SOCKET");
    }
    if(cmptr->cmsg_type != SCM_RIGHTS){
      err_quit("control type != SCM_RIGHTS");
    }
    *recvfd = *((int *) CMSG_DATA(cmptr));

  }else{
    *recvfd = -1;  // descriptor was not passed
  }
#else
  if(msg.msg_accrightslen == sizeof(int)){
    *recvfd = newfd;
  }else{
    *recvfd = -1;  // descriptor was not passed
  }
#endif
  
  return cnt;
}



ssize_t _read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
  ssize_t cnt;  
  if(0 > (cnt = read_fd(fd, ptr, nbytes, recvfd))){
    err_sys("read_fd error");
  }
  return cnt;
}



/*
  signal_intr.c
*/
Sigfunc* signal_intr(int signo, Sigfunc *func)
{
  struct sigaction act, oact;
  
  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

#ifdef SA_INTERRUPT // SunOS
  act.sa_flags |= SA_INTERRUPT;
#endif

  if(0 > sigaction(signo, &act, &oact)){
    return SIG_ERR;
  }
  return oact.sa_handler;
}



Sigfunc* _signal_intr(int signo, Sigfunc *func)
{
  Sigfunc *sigfunc;
  
  if(SIG_ERR == (sigfunc = signal_intr(signo, func))){
    err_sys("signal_intr error");
  }
  return sigfunc;
}



/*
  sock_bind_wild.c
*/
int sock_bind_wild(int sockfd, int family)
{
  socklen_t len;
  
  switch (family) {
  case AF_INET: 
    {
      struct sockaddr_in sin;      

      bzero(&sin, sizeof(sin));
      sin.sin_family = AF_INET;
      sin.sin_addr.s_addr = htonl(INADDR_ANY);
      sin.sin_port = htons(0); // bind ephemeral port 
      
      if(0 > bind(sockfd, (SA *) &sin, sizeof(sin))){
        return -1;
      }
 
      len = sizeof(sin);
      
      if(0 > getsockname(sockfd, (SA *) &sin, &len)){
        return -1;
      }
      return sin.sin_port;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      struct sockaddr_in6 sin6;      

      bzero(&sin6, sizeof(sin6));
      sin6.sin6_family = AF_INET6;
      sin6.sin6_addr = in6addr_any;
      sin6.sin6_port = htons(0); // bind ephemeral port
      
      if(0 > bind(sockfd, (SA *) &sin6, sizeof(sin6))){
        return -1;
      }

      len = sizeof(sin6);

      if(0 > getsockname(sockfd, (SA *) &sin6, &len)){
        return -1;
      }

      return sin6.sin6_port;
    }
#endif
  }
  return(-1);
}


int _sock_bind_wild(int sockfd, int family)
{
  int port;
  
  if(0 > (port = sock_bind_wild(sockfd, family))){
    err_sys("sock_bind_wild error");
  }
  
  return(port);
}



/*
  sock_cmp_addr.c
*/
int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
  if(sa1->sa_family != sa2->sa_family){
    return(-1);
  }
  
  switch (sa1->sa_family){
  case AF_INET: 
    {
      return memcmp(&((struct sockaddr_in *) sa1)->sin_addr,
                    &((struct sockaddr_in *) sa2)->sin_addr,
                    sizeof(struct in_addr));
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      return memcmp(&((struct sockaddr_in6 *) sa1)->sin6_addr,
                    &((struct sockaddr_in6 *) sa2)->sin6_addr,
                    sizeof(struct in6_addr));
    }
#endif
    
#ifdef AF_UNIX
  case AF_UNIX: 
    {
      return strcmp(((struct sockaddr_un *) sa1)->sun_path,
                    ((struct sockaddr_un *) sa2)->sun_path);
    }
#endif
    
#ifdef HAVE_SOCKADDR_DL_STRUCT
  case AF_LINK: 
    {
      return -1;  // no idea what to compare here ? 
    }
#endif
  }
  return -1;
}



/*
  sock_cmp_port.c
*/
int sock_cmp_port(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
  if(sa1->sa_family != sa2->sa_family){
    return(-1);
  }
  
  switch (sa1->sa_family){
  case AF_INET: 
    {
      return( ((struct sockaddr_in *) sa1)->sin_port ==
              ((struct sockaddr_in *) sa2)->sin_port);
    }
    
#ifdef IPV6
  case AF_INET6:
    {
      return( ((struct sockaddr_in6 *) sa1)->sin6_port ==
              ((struct sockaddr_in6 *) sa2)->sin6_port);
    }
#endif
    
  }
  return (-1);
}



/*
  sock_get_port.c
*/
int sock_get_port(const struct sockaddr *sa, socklen_t salen)
{
  switch (sa->sa_family) {
  case AF_INET: 
    {
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;
      return(sin->sin_port);
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
      return(sin6->sin6_port);
    }
#endif
  }

  return -1;
}



/*
  sock_set_addr.c
*/
void sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr)
{
  switch (sa->sa_family) {
  case AF_INET: 
    {
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;
      memcpy(&sin->sin_addr, addr, sizeof(struct in_addr));
      return;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
      memcpy(&sin6->sin6_addr, addr, sizeof(struct in6_addr));
      return;
    }
#endif
  }
  
  return;
}



/*
  sock_set_port.c
*/
void sock_set_port(struct sockaddr *sa, socklen_t salen, int port)
{
  switch (sa->sa_family){
  case AF_INET: 
    {
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;
      sin->sin_port = port;
      return;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;     
      sin6->sin6_port = port;
      return;
    }
#endif
  }
  
  return;
}



/*
  sock_set_wild.c
*/
void sock_set_wild(struct sockaddr *sa, socklen_t salen)
{
  const void *wildptr;
  
  switch (sa->sa_family) {
  case AF_INET: 
    {
      static struct in_addr in4addr_any;
      
      in4addr_any.s_addr = htonl(INADDR_ANY);
      wildptr = &in4addr_any;
      break;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      wildptr = &in6addr_any; // in6addr_any.c
      break;
    }
#endif
    
  default:
    return;
  }
  sock_set_addr(sa, salen, wildptr);
  return;
}



/*
  sock_ntop.c
*/
char* sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
  char portstr[8];
  static char str[128];  // Unix domain is largest
  
  switch (sa->sa_family){
  case AF_INET: 
    {
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;
      if(NULL == inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))){
        return(NULL);
      }
      if(0 != ntohs(sin->sin_port)){
        snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
        strcat(str, portstr);
      }
      return str;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
      str[0] = '[';
      if(NULL == inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1)){
        return(NULL);
      }
      if(0 != ntohs(sin6->sin6_port)){
        snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
        strcat(str, portstr);
        return str;
      }
      return (str + 1);
    }
#endif
    
#ifdef AF_UNIX
  case AF_UNIX: 
    {
      struct sockaddr_un *unp = (struct sockaddr_un *) sa;
    
      /* 
         OK to have no pathname bound to the socket: happens on
         every connect() unless client calls bind() first. 
      */
      if(0 == unp->sun_path[0]){
        strcpy(str, "(no pathname bound)");
      }else{
        snprintf(str, sizeof(str), "%s", unp->sun_path);
      }
      return str;
    }
#endif
    
#ifdef HAVE_SOCKADDR_DL_STRUCT
  case AF_LINK: 
    {
      struct sockaddr_dl *sdl = (struct sockaddr_dl *) sa;      
      if(sdl->sdl_nlen > 0){
        snprintf(str
                 , sizeof(str)
                 , "%*s (index %d)"
                 , sdl->sdl_nlen
                 , &sdl->sdl_data[0]
                 , sdl->sdl_index);
      }else{
        snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
      }
      return str;
  }
#endif

  default:
    snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
    return(str);
  }
  return NULL;
}



char* _sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
  char *ptr = NULL;  
  if(NULL == (ptr = sock_ntop(sa, salen))){
    err_sys("sock_ntop error"); /* inet_ntop() sets errno */
  }
  return ptr;
}



/*
  sock_ntop_host.c
*/
char* sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
  static char str[128];  // Unix domain is largest
  
  switch (sa->sa_family){
  case AF_INET: 
    {
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;      
      if(NULL == inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))){
        return(NULL);
      }
      return str;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
      if(NULL == inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str))){
        return(NULL);
      }
      return str;
    }
#endif
    
#ifdef AF_UNIX
  case AF_UNIX: 
    {
      struct sockaddr_un *unp = (struct sockaddr_un *) sa;
      /* 
         OK to have no pathname bound to the socket: happens on
         every connect() unless client calls bind() first. 
      */
      if(0 == unp->sun_path[0]){
        strcpy(str, "(no pathname bound)");
      }else{
        snprintf(str, sizeof(str), "%s", unp->sun_path);
      }
      return str;
    }
#endif
    
#ifdef HAVE_SOCKADDR_DL_STRUCT
  case AF_LINK:
    {
      struct sockaddr_dl *sdl = (struct sockaddr_dl *) sa;
      
      if(0 < sdl->sdl_nlen){
        snprintf(str, sizeof(str), "%*s", sdl->sdl_nlen, &sdl->sdl_data[0]);
      }else{
        snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
      }
      return str;
    }
#endif
  default:
    snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
    return str;
  }
  return NULL;
}



char* _sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
  char *ptr = NULL;  
  if(NULL == (ptr = sock_ntop_host(sa, salen))){
    err_sys("sock_ntop_host error"); // inet_ntop() sets errno 
  }
  return ptr;
}



/*
  sockfd_to_family.c
*/
int sockfd_to_family(int sockfd)
{
  struct sockaddr_storage ss;
  socklen_t len;
  
  len = sizeof(ss);
  if(0 > getsockname(sockfd, (SA *) &ss, &len)){
    return -1;
  }
  return ss.ss_family;
}



int _sockfd_to_family(int sockfd)
{
  int rc;  
  if(0 > (rc = sockfd_to_family(sockfd))){
    err_sys("sockfd_to_family error");
  }
  
  return rc;
}



/*
  str_echo.c
*/
void str_echo(int sockfd)
{
  ssize_t res;
  char buf[MAXLINE];
  
 again:
  while(0 < (res = read(sockfd, buf, MAXLINE))){
    _writen(sockfd, buf, res);
  }
  
  if((0 > res) && (errno == EINTR)){
    goto again;
  }else if(0 > res){
    err_sys("str_echo: read error");
  }
}



/*
  str_cli.c
*/
void str_cli(FILE *fp, int sockfd)
{
  char sendline[MAXLINE], recvline[MAXLINE];
  
  while(NULL != _fgets(sendline, MAXLINE, fp)){
    
    _writen(sockfd, sendline, strlen(sendline));    
    if(0 == _readline(sockfd, recvline, MAXLINE)){
      err_quit("str_cli: server terminated prematurely");
    }    
    _fputs(recvline, stdout);
  }
}



/*
  tcp_connect.c
*/
int tcp_connect(const char *host, const char *serv)
{
  int sockfd, eai;
  struct addrinfo hints, *res, *ressave;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if(0 != (eai = getaddrinfo(host, serv, &hints, &res))){
    err_quit("tcp_connect error for %s, %s: %s", host, serv, gai_strerror(eai));
  }

  ressave = res;
  
  do{
    if(0 > (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol))){
      continue; // ignore this one
    }
    
    if(0 == connect(sockfd, res->ai_addr, res->ai_addrlen)){
      break;  // success
    }
    
    _close(sockfd); // ignore this one 
  }while(NULL != (res = res->ai_next));
  
  if(NULL == res){ // errno set from final connect()
    err_sys("tcp_connect error for %s, %s", host, serv);
  }

  freeaddrinfo(ressave);
  
  return(sockfd);
}



/*
  We place the wrapper function here, not in wraplib.c, because some
  XTI programs need to include wraplib.c, and it also defines
  a Tcp_connect() function.
*/
int _tcp_connect(const char *host, const char *serv)
{
  return(tcp_connect(host, serv));
}



/*
  tcp_listen.c
*/
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
  int listenfd, eai;
  const int  on = 1;
  struct addrinfo hints, *res, *ressave;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if(0 != (eai = getaddrinfo(host, serv, &hints, &res))){
    err_quit("tcp_listen error for %s, %s: %s", host, serv, gai_strerror(eai));
  }

  ressave = res;
  
  do{
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(0 > listenfd){
      continue; // error, try next one
    }   
    _setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(0 == bind(listenfd, res->ai_addr, res->ai_addrlen)){
      break; // success
    }
    
    _close(listenfd); // bind error, close and try next one 
  }while(NULL != (res = res->ai_next));
  
  if(NULL == res){ // errno from final socket() or bind()
    err_sys("tcp_listen error for %s, %s", host, serv);
  }
  
  _listen(listenfd, LISTENQ);
  
  if(addrlenp){
    *addrlenp = res->ai_addrlen; // return size of protocol address
  }
  
  freeaddrinfo(ressave);
  
  return(listenfd);
}



/*
  We place the wrapper function here, not in wraplib.c, because some
  XTI programs need to include wraplib.c, and it also defines
  a Tcp_listen() function.
*/
int _tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
  return(tcp_listen(host, serv, addrlenp));
}



/*
  tv_sub
*/
void tv_sub(struct timeval *out, struct timeval *in)
{
  if(0 > (out->tv_usec -= in->tv_usec)){ // out -= in
    --out->tv_sec;
    out->tv_usec += 1000000;
  }
  out->tv_sec -= in->tv_sec;
}



/*
  udp_client.c
*/
int udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenp)
{
  int sockfd, eai;
  struct addrinfo hints, *res=NULL, *ressave=NULL;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  
  if(0 != (eai = getaddrinfo(host, serv, &hints, &res))){
    err_quit("udp_client error for %s, %s: %s", host, serv, gai_strerror(eai));
  }
  ressave = res;
  
  do{
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd >= 0)
      break;  // success
  }while(NULL != (res = res->ai_next));
  
  if(res == NULL){ // errno set from final socket()
    err_sys("udp_client error for %s, %s", host, serv);
  } 
 
  *saptr = _malloc(res->ai_addrlen);
  memcpy(*saptr, res->ai_addr, res->ai_addrlen);
  *lenp = res->ai_addrlen;
  
  freeaddrinfo(ressave);
  
  return(sockfd);
}



int _udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenptr)
{
  return(udp_client(host, serv, saptr, lenptr));
}



/*
  udp_connect.c
*/
int udp_connect(const char *host, const char *serv)
{
  int sockfd, eai;
  struct addrinfo hints, *res=NULL, *ressave=NULL;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  
  if(0 != (eai = getaddrinfo(host, serv, &hints, &res))){
    err_quit("udp_connect error for %s, %s: %s", host, serv, gai_strerror(eai));
  }

  ressave = res;
  
  do{
    if(0 > (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol))){
      continue; // ignore this one
    }
    
    if(0 == connect(sockfd, res->ai_addr, res->ai_addrlen)){
      break;  // success 
    }
    
    _close(sockfd); // ignore this one
  }while(NULL != (res = res->ai_next));
  
  if(res == NULL){ // errno set from final connect()
    err_sys("udp_connect error for %s, %s", host, serv);
  }
  
  freeaddrinfo(ressave);
  
  return(sockfd);
}



int _udp_connect(const char *host, const char *serv)
{
  int res;
  if(0 > (res = udp_connect(host, serv))){
    err_quit("udp_connect error for %s, %s: %s", host, serv, gai_strerror(-res));
  }
  return(res);
}



/*
  udp_server.c
*/
int udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
  int sockfd, eai;
  struct addrinfo hints, *res=NULL, *ressave=NULL;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  
  if(0 != (eai = getaddrinfo(host, serv, &hints, &res))){
    err_quit("udp_server error for %s, %s: %s", host, serv, gai_strerror(eai));
  }

  ressave = res;
  
  do{
    if(0 > (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol))){
      continue; // error - try next one
    }
    
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == 0){
      break; // success 
    }
    
    _close(sockfd);  // bind error - close and try next one
  }while(NULL != (res = res->ai_next));
  
  if(res == NULL){ // errno from final socket() or bind()
    err_sys("udp_server error for %s, %s", host, serv);
  }

  if(addrlenp){
    *addrlenp = res->ai_addrlen; // return size of protocol address
  }
  
  freeaddrinfo(ressave);
  
  return(sockfd);
}



int _udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
  return(udp_server(host, serv, addrlenp));
}



/*
  writable_timeo.c
*/
int writable_timeo(int fd, int sec)
{
  fd_set wset;
  struct timeval tv;
  
  FD_ZERO(&wset);
  FD_SET(fd, &wset);
  
  tv.tv_sec = sec;
  tv.tv_usec = 0;
  
  return(select(fd+1, NULL, &wset, NULL, &tv));
  // > 0 if descriptor is writable 
}



int _writable_timeo(int fd, int sec)
{
  int  bytes;
  
  if(0 > (bytes = writable_timeo(fd, sec))){
    err_sys("writable_timeo error");
  }
  return(bytes);
}



/*
  writen.c

  Write "num" bytes to a descriptor. 
*/
ssize_t writen(int fd, const void *vptr, size_t num)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;
  
  ptr = vptr;
  nleft = num;
  while(0 < nleft){
    if(0 >= (nwritten = write(fd, ptr, nleft))){
      if((nwritten < 0) && (errno == EINTR)){
        // and call write() again
        nwritten = 0;
      }else{
        // error
        return(-1);   
      }
    }
    
    nleft -= nwritten;
    ptr += nwritten;
  }
  return num;
}


void _writen(int fd, void *ptr, size_t nbytes)
{
  if(writen(fd, ptr, nbytes) != nbytes){
    err_sys("writen error");
  }
}



/*
  write_fd.c
*/
ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
  struct msghdr msg;
  struct iovec iov[1];
  
#ifdef HAVE_MSGHDR_MSG_CONTROL
  union{
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
#else
  msg.msg_accrights = (caddr_t) &sendfd;
  msg.msg_accrightslen = sizeof(int);
#endif
  
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  
  iov[0].iov_base = ptr;
  iov[0].iov_len = nbytes;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  
  return(sendmsg(fd, &msg, 0));
}


ssize_t _write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
  ssize_t  bytes;
  
  if(0 > (bytes = write_fd(fd, ptr, nbytes, sendfd))){
    err_sys("write_fd error");
  }
  
  return(bytes);
}



/*
  signal.c
*/
Sigfunc* signal(int signo, Sigfunc *func)
{
  struct sigaction act, oact;
  
  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  if(signo == SIGALRM){
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
#endif
  }else{
#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART;  // SVR4, 44BSD 
#endif
  }

  if(0 > sigaction(signo, &act, &oact)){
    return SIG_ERR;
  }
  return oact.sa_handler;
}



Sigfunc* _signal(int signo, Sigfunc *func) // for our signal() function 
{
  Sigfunc *sigfunc=NULL;
  
  if(SIG_ERR == (sigfunc = signal(signo, func))){
    err_sys("signal error");
  }
  return sigfunc;
}



/*
  sockatmark.c
*/
int sockatmark(int fd)
{
  int  flag;
  
  if(0 > ioctl(fd, SIOCATMARK, &flag)){
    return -1;
  }
  return (flag != 0);
}
