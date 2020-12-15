// wrap_sock.c
/*
  Socket wrapper functions.
  These could all go into separate files, so only the ones needed cause
  the corresponding function to be added to the executable.  If sockets
  are a library (SVR4) this might make a difference (?), but if sockets
  are in the kernel (BSD) it doesn't matter.

  These wrapper functions also use the same prototypes as POSIX.1g,
  which might differ from many implementations (i.e., POSIX.1g specifies
  the fourth argument to getsockopt() as "void *", not "char *").
 
  If your system's headers are not correct [i.e., the Solaris 2.5
  <sys/socket.h> omits the "const" from the second argument to both
  bind() and connect()], you'll get warnings of the form:
  
  warning: passing arg 2 of `bind' discards `const' from pointer target type
  warning: passing arg 2 of `connect' discards `const' from pointer target type
 */

#include "lib_socket.h"


int _accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  int res;
  
 again:
  if(0 > (res = accept(fd, sa, salenptr))){
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
  return(res);
}



void _bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if(0 > bind(fd, sa, salen)){
    err_sys("bind error");
  }
}



void _connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if(0 > connect(fd, sa, salen)){
    err_sys("connect error");
  }
}



void _getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if(0 > getpeername(fd, sa, salenptr)){
    err_sys("getpeername error");
  }
}



void _getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if(0 > getsockname(fd, sa, salenptr)){
    err_sys("getsockname error");
  }
}



void _getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
  if(0 > getsockopt(fd, level, optname, optval, optlenptr)){
    err_sys("getsockopt error");
  }
}


/* TODO fix config.h - HP-UX
#ifdef HAVE_INET6_RTH_INIT
int _inet6_rth_space(int type, int segments)
{
  int res;  
  if(0 > (res = inet6_rth_space(type, segments))){
    err_quit("inet6_rth_space error");
  }
  return res;
}



void* _inet6_rth_init(void *rthbuf, socklen_t rthlen, int type, int segments)
{
  void *res = NULL;
  if(NULL == (res = inet6_rth_init(rthbuf, rthlen, type, segments))){
    err_quit("inet6_rth_init error");
  }  
  return res;
}



void _inet6_rth_add(void *rthbuf, const struct in6_addr *addr)
{
  if(0 > inet6_rth_add(rthbuf, addr)){
    err_quit("inet6_rth_add error");
  }
}



void _inet6_rth_reverse(const void *in, void *out)
{
  if(0 > inet6_rth_reverse(in, out)){
    err_quit("inet6_rth_reverse error");
  }
}



int _inet6_rth_segments(const void *rthbuf)
{
  int res;  
  if(0 > (res = inet6_rth_segments(rthbuf))){
    err_quit("inet6_rth_segments error");
  }
  return res;
}



struct in6_addr* _inet6_rth_getaddr(const void *rthbuf, int idx)
{
  struct in6_addr *res = NULL;  
  if(NULL == (res = inet6_rth_getaddr(rthbuf, idx))){
    err_quit("inet6_rth_getaddr error");
  }
  return res;
}
#endif
//*/ // HP-UX


#ifdef HAVE_KQUEUE
int _kqueue(void)
{
  int res;  
  if(0 > (res = kqueue())){
    err_sys("kqueue error");
  }
  return res;
}



int _kevent(int kq
            , const struct kevent* changelist
            , int nchanges
            , struct kevent* eventlist
            , int nevents
            , const struct timespec* timeout)
{
  int res;  
  if(0 > (res = kevent(kq, changelist, nchanges, eventlist, nevents, timeout))){
    err_sys("kevent error");
  }
  return res;
}
#endif




void _listen(int fd, int backlog)
{
  char *ptr = NULL;
  
  // 4can override 2nd argument with environment variable
  if(NULL != (ptr = getenv("LISTENQ"))){
    backlog = atoi(ptr);
  }
  
  if(0 > listen(fd, backlog)){
    err_sys("listen error");
  }
}



#ifdef HAVE_POLL
int _poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
  int res;
  if(0 > (res = poll(fdarray, nfds, timeout))){
    err_sys("poll error");
  }
  return(res);
}
#endif



ssize_t _recv(int fd, void *ptr, size_t nbytes, int flags)
{
  ssize_t res;  
  if(0 > (res = recv(fd, ptr, nbytes, flags))){
    err_sys("recv error");
  }
  return(res);
}



ssize_t _recvfrom(int fd
                  , void *ptr
                  , size_t nbytes
                  , int flags
                  , struct sockaddr *sa
                  , socklen_t *salenptr)
{
  ssize_t  res;
  if(0 > (res = recvfrom(fd, ptr, nbytes, flags, sa, salenptr))){
    err_sys("recvfrom error");
  }
  return(res);
}



ssize_t _recvmsg(int fd, struct msghdr *msg, int flags)
{
  ssize_t  res; 
  if(0 > (res = recvmsg(fd, msg, flags))){
    err_sys("recvmsg error");
  }
  return(res);
}



int _select(int nfds
            , fd_set *readfds
            , fd_set *writefds
            , fd_set *exceptfds
            , struct timeval *timeout)
{
  int  res;  
  if(0 > (res = select(nfds, readfds, writefds, exceptfds, timeout))){
    err_sys("select error");
  }
  return(res);  /* can return 0 on timeout */
}



void _send( int fd
            , const void *ptr
            , size_t nbytes
            , int flags)
{
  if(send(fd, ptr, nbytes, flags) != (ssize_t)nbytes){
    err_sys("send error");
  }
}



void _sendto( int fd
             , const void *ptr
             , size_t nbytes
             , int flags
             , const struct sockaddr *sa
             , socklen_t salen)
{
  if(sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t) nbytes){
    err_sys("sendto error");
  }
}



void _sendmsg(int fd, const struct msghdr *msg, int flags)
{
  unsigned int idx;
  ssize_t nbytes;
  
  nbytes = 0; /* must first figure out what return value should be */
  for(idx = 0; idx < msg->msg_iovlen; ++idx){
    nbytes += msg->msg_iov[idx].iov_len;
  }
 
  if(nbytes != sendmsg(fd, msg, flags)){
    err_sys("sendmsg error");
  }
}



void _setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
  if(0 > setsockopt(fd, level, optname, optval, optlen)){
    err_sys("setsockopt error");
  }
}



void _shutdown(int fd, int how)
{
  if(0 > shutdown(fd, how)){
    err_sys("shutdown error");
  }
}



int _sockatmark(int fd)
{
  int res;
  if(0 > (res = sockatmark(fd))){
    err_sys("sockatmark error");
  }
  return res;
}

/*
  include Socket 
*/
int _socket(int family, int type, int protocol)
{
  int res;
  if(0 > (res = socket(family, type, protocol))){
    err_sys("socket error");
  }
  return res;
}


void _socketpair(int family, int type, int protocol, int *fd)
{
  if(0 > socketpair(family, type, protocol, fd)){
    err_sys("socketpair error");
  }
}
