/*
  sock

  socket wrapper functions to Linux / POSIX library calls

  Lothar Rubusch

  ---
  References:
  Unix Network Programming, Stevens
*/

#include "lib_socket.h"


/*
  The accept() function shall extract the first connection on the
  queue of pending connections, create a new socket with the same
  socket type protocol and address family as the specified socket, and
  allocate a new file descriptor for that socket.

  #include <sys/socket.h>

  @fd: Specifies a socket that was created with socket(), has been
      bound to an address with bind(), and has issued a successful
      call to listen().
  @sa: Either a null pointer, or a pointer to a sockaddr structure
      where the address of the connecting socket shall be returned.
  @salenptr: Points to a socklen_t structure which on input specifies
      the length of the supplied sockaddr structure, and on output
      specifies the length of the stored address.
*/
int lothars__accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int res;
again:
	if (0 > (res = accept(fd, sa, salenptr))) {
		if ((errno == EPROTO) || (errno == ECONNABORTED)) {
			goto again;
		} else {
			err_sys("%s() error", __func__);
		}
	}
	return res;
}


/*
  The bind() function shall assign a local socket address address to a
  socket identified by descriptor socket that has no local socket
  address assigned. Sockets created with the socket() function are
  initially unnamed; they are identified only by their address family.

  #include <sys/types.h>
  #include <sys/socket.h>

  @fd: Specifies the file descriptor of the socket to be bound.
  @sa: Points to a sockaddr structure containing the address to be
      bound to the socket. The length and format of the address depend
      on the address family of the socket.
  @salen: Specifies the length of the sockaddr structure pointed to by
      the address argument.
*/
void lothars__bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > bind(fd, sa, salen)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The connect() function shall attempt to make a connection on a socket.

  #include <sys/socket.h>

  @fd: Specifies the file descriptor associated with the socket.
  @sa: Points to a sockaddr structure containing the peer address. The
      length and format of the address depend on the address family of
      the socket.
  @salen: Specifies the length of the sockaddr structure pointed to by
      the address argument.
*/
void lothars__connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > connect(fd, sa, salen)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The getpeername() function shall retrieve the peer address of the
  specified socket, store this address in the sockaddr structure
  pointed to by the address argument, and store the length of this
  address in the object pointed to by the address_len argument.

  #include <sys/socket.h>

  @fd: The socket file descriptor to provide the peer address.
  @sa: The address argument to store the address.
  @address_len: The length of the address element.
*/
void lothars__getpeername(int fd, struct sockaddr *address, socklen_t *address_len)
{
	if (0 > getpeername(fd, address, address_len)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The getsockname() function shall retrieve the locally-bound name of
  the specified socket, store this address in the sockaddr structure
  pointed to by the address argument, and store the length of this
  address in the object pointed to by the address_len argument.

  #include <sys/socket.h>

  @fd: The socket file descriptor to provide the peer address.
  @sa: The address argument to store the address.
  @address_len: The length of the address element.
*/
void lothars__getsockname(int fd, struct sockaddr *address, socklen_t *address_len)
{
	if (0 > getsockname(fd, address, address_len)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The getsockopt() function shall retrieve the value for the option
  specified by the option_name argument for the socket specified by
  the socket argument. If the size of the option value is greater than
  option_len, the value stored in the object pointed to by the
  option_value argument shall be silently truncated. Otherwise, the
  object pointed to by the option_len argument shall be modified to
  indicate the actual length of the value.

  #include <sys/socket.h>

  @fd: The socket file descriptor to provide the peer address.
  @level: The level argument specifies the protocol level at which the
      option resides.
  @optname: The option_name argument specifies a single option to be
      retrieved.
  @optval: The value stored in the object pointed to.
  @optlenptr: The option_len argument shall be modified to indicate
      the actual length of the value.
*/
void lothars__getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
	if (0 > getsockopt(fd, level, optname, optval, optlenptr)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The listen() function shall mark a connection-mode socket, specified
  by the socket argument, as accepting connections.

  #include <sys/socket.h>

  @fd: Specifies the file descriptor associated with the socket.
  @backlog: The backlog argument provides a hint to the implementation
      which the implementation shall use to limit the number of
      outstanding connections in the socket's listen queue.
*/
void lothars__listen(int fd, int backlog)
{
	char *ptr = NULL;
	// can override 2nd argument with environment variable
	if (NULL != (ptr = getenv("LISTENQ"))) {
		backlog = atoi(ptr);
	}

	if (0 > listen(fd, backlog)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The poll() function provides applications with a mechanism for
  multiplexing input/output over a set of file descriptors. For each
  member of the array pointed to by fds, poll() shall examine the
  given file descriptor for the event(s) specified in events. The
  number of pollfd structures in the fds array is specified by
  nfds. The poll() function shall identify those file descriptors on
  which an application can read or write data, or on which certain
  events have occurred.

  #include <poll.h>

  @fdarray: The fds argument specifies the file descriptors to be
      examined and the events of interest for each file descriptor.
  @nfds: The numer of fdarray [fds].
  @timeout: Alternatively, provide a timeout.
*/
int lothars__poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
	int res;
	if (0 > (res = poll(fdarray, nfds, timeout))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  The recv() function shall receive data from a connection-mode or
  connectionless-mode socket. It is normally used with connected
  sockets because it does not permit the application to retrieve the
  source address of received data.

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor.
  @ptr: Points to a buffer where the message should be stored.
  @nbytes: Specifies the length in bytes of the buffer pointed to by
      the buffer argument.
  @flags: Specifies the type of message reception. Values of this
      argument are formed by logically OR'ing zero or more of the
      following values:

      MSG_PEEK
        Peeks at an incoming message. The data is treated as unread
        and the next recv() or similar function shall still return
        this data.

      MSG_OOB
        Requests out-of-band data. The significance and semantics of
        out-of-band data are protocol-specific.

      MSG_WAITALL
        On SOCK_STREAM sockets this requests that the function block
        until the full amount of data can be returned. The function
        may return the smaller amount of data if the socket is a
        message-based socket, if a signal is caught, if the connection
        is terminated, if MSG_PEEK was specified, or if an error is
        pending for the socket.
*/
ssize_t lothars__recv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t res;
	if (0 > (res = recv(fd, ptr, nbytes, flags))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  The recvfrom() function shall receive a message from a
  connection-mode or connectionless-mode socket. It is normally used
  with connectionless-mode sockets because it permits the application
  to retrieve the source address of received data.

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor.
  @ptr: Points to the buffer where the message should be stored.
  @nbytes: Specifies the length in bytes of the buffer pointed to by
      the buffer argument.
  @flags: Specifies the type of message reception.
  @sa: A null pointer, or points to a sockaddr structure in which the
      sending address is to be stored.
  @salenptr: Specifies the length of the sockaddr structure pointed to
      by the address argument.
*/
ssize_t lothars__recvfrom(int fd
		  , void *ptr
		  , size_t nbytes
		  , int flags
		  , struct sockaddr *sa
		  , socklen_t *salenptr)
{
	ssize_t  res;
	if (0 > (res = recvfrom(fd, ptr, nbytes, flags, sa, salenptr))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  The recvmsg() function shall receive a message from a
  connection-mode or connectionless-mode socket. It is normally used
  with connectionless-mode sockets because it permits the application
  to retrieve the source address of received data.

  #include <sys/socket.h>

  @fd: The socket file descriptor.
  @msg: Points to a msghdr structure, containing both the buffer to
      store the source address and the buffers for the incoming
      message. The length and format of the address depend on the
      address family of the socket. The msg_flags member is ignored on
      input, but may contain meaningful values on output.
  @flags: Specifies the type of message reception.
*/
ssize_t lothars__recvmsg(int fd, struct msghdr *msg, int flags)
{
	ssize_t  res;
	if (0 > (res = recvmsg(fd, msg, flags))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  The pselect() function shall examine the file descriptor sets whose
  addresses are passed in the readfds, writefds, and errorfds
  parameters to see whether some of their descriptors are ready for
  reading, are ready for writing, or have an exceptional condition
  pending, respectively.

  The select() function shall be equivalent to the pselect() function,
  except as follows:

  - For the select() function, the timeout period is given in seconds
    and microseconds in an argument of type struct timeval, whereas
    for the pselect() function the timeout period is given in seconds
    and nanoseconds in an argument of type struct timespec.

  - The select() function has no sigmask argument; it shall behave as
    pselect() does when sigmask is a null pointer.

  - Upon successful completion, the select() function may modify the
    object pointed to by the timeout argument.

  #include <sys/select.h>

  @nfds: The nfds argument specifies the range of descriptors to be
      tested. The first nfds descriptors shall be checked in each set;
      that is, the descriptors from zero through nfds-1 in the
      descriptor sets shall be examined.
  @readfs: If the readfds argument is not a null pointer, it points to
      an object of type fd_set that on input specifies the file
      descriptors to be checked for being ready to read, and on output
      indicates which file descriptors are ready to read.
  @writefds: If the writefds argument is not a null pointer, it points
      to an object of type fd_set that on input specifies the file
      descriptors to be checked for being ready to write, and on
      output indicates which file descriptors are ready to write.
  @exceptfds: If the errorfds argument is not a null pointer, it
      points to an object of type fd_set that on input specifies the
      file descriptors to be checked for error conditions pending, and
      on output indicates which file descriptors have error conditions
      pending.
  @timeout: The timeout parameter controls how long the pselect() or
      select() function shall take before timing out. If the timeout
      parameter is not a null pointer, it specifies a maximum interval
      to wait for the selection to complete.
*/
int lothars__select(int nfds
	    , fd_set *readfds
	    , fd_set *writefds
	    , fd_set *exceptfds
	    , struct timeval *timeout)
{
	int  res;
	if (0 > (res = select(nfds, readfds, writefds, exceptfds, timeout))) {
		err_sys("%s() error", __func__);
	}
	return res;  /* can return 0 on timeout */
}


/*
  The send() function shall initiate transmission of data from the
  specified socket to its peer. The send() function shall send a
  message only when the socket is connected (including when the peer
  of a connectionless socket has been set via connect()).

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor.
  @ptr: Points to the buffer containing the message to send.
  @nbytes: Specifies the length of the message in bytes.
  @flags: Specifies the type of message transmission. Values of this
      argument are formed by logically OR'ing zero or more of the
      following flags:

        MSG_EOR
          Terminates a record (if supported by the protocol).

	MSG_OOB
          Sends out-of-band data on sockets that support out-of-band
          communications. The significance and semantics of
          out-of-band data are protocol-specific.
*/
void lothars__send( int fd
	    , const void *ptr
	    , size_t nbytes
	    , int flags)
{
	if ((ssize_t) nbytes != send(fd, ptr, nbytes, flags)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The sendto() function shall send a message through a connection-mode
  or connectionless-mode socket. If the socket is connectionless-mode,
  the message shall be sent to the address specified by dest_addr. If
  the socket is connection-mode, dest_addr shall be ignored.

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor.
  @ptr: Points to a buffer containing the message to be sent.
  @nbytes: Specifies the size of the message in bytes.
  @flags: Specifies the type of message transmission.
  @sa: Points to a sockaddr structure containing the destination
      address.
  @salen: Specifies the length of the sockaddr structure pointed to by
      the dest_addr argument.
*/
void lothars__sendto( int fd
	      , const void *ptr
	      , size_t nbytes
	      , int flags
	      , const struct sockaddr *sa
	      , socklen_t salen)
{
	if ((ssize_t) nbytes != sendto(fd, ptr, nbytes, flags, sa, salen)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The sendmsg() function shall send a message through a
  connection-mode or connectionless-mode socket. If the socket is
  connectionless-mode, the message shall be sent to the address
  specified by msghdr. If the socket is connection-mode, the
  destination address in msghdr shall be ignored.

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor.
  @msg: Points to a msghdr structure, containing both the destination
      address and the buffers for the outgoing message. The length and
      format of the address depend on the address family of the
      socket. The msg_flags member is ignored.
  @flags: Specifies the type of message transmission.
*/
void lothars__sendmsg(int fd, const struct msghdr *msg, int flags)
{
	int idx;
	ssize_t nbytes;

	nbytes = 0; /* must first figure out what return value should be */
	for (idx = 0; idx < msg->msg_iovlen; ++idx) {
		nbytes += msg->msg_iov[idx].iov_len;
	}

	if (nbytes != sendmsg(fd, msg, flags)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The setsockopt() function shall set the option specified by the
  option_name argument, at the protocol level specified by the level
  argument, to the value pointed to by the option_value argument for
  the socket associated with the file descriptor specified by the
  socket argument.

  #include <sys/socket.h>

  @fd: The socket file descriptor to provide the peer address.
  @level: The level argument specifies the protocol level at which the
      option resides.
  @optname: The option_name argument specifies a single option to be
      set.
  @optval: The value stored in the object pointed to.
  @optlenptr: The option_len argument shall be modified to indicate
      the actual length of the value.
*/
void lothars__setsockopt(int fd
			 , int level
			 , int optname
			 , const void *optval
			 , socklen_t optlen)
{
	if (0 > setsockopt(fd, level, optname, optval, optlen)) {
//		lothars__close(fd_sock); /* strict */
		err_sys("%s() error", __func__);
	}
}


/*
  The shutdown() function shall cause all or part of a full-duplex
  connection on the socket associated with the file descriptor socket
  to be shut down.

  #include <sys/socket.h>

  @fd: Specifies the file descriptor of the socket.
  @how: Specifies the type of shutdown. (see manpages)
*/
void lothars__shutdown(int fd, int how)
{
	if (0 > shutdown(fd, how)) {
		err_sys("%s() error", __func__);
	}
}


/*
  sockatmark - determine whether socket is at out-of-band mark.

  sockatmark() returns a value indicating whether or not the socket
  referred to by the file descriptor sockfd is at the out-of-band
  mark. If the socket is at the mark, then 1 is returned; if the
  socket is not at the mark, 0 is returned. This function does not
  remove the out-of-band mark.

  #ifdef _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
  #include <sys/socket.h>

  @fd: The specific socket descriptor.
*/
int lothars__sockatmark(int fd)
{
	int res;
	if (0 > (res = sockatmark(fd))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  The socket() function shall create an unbound socket in a
  communications domain, and return a file descriptor that can be used
  in later function calls that operate on sockets.

  #include <sys/socket.h>

  @family: Specifies the communications domain in which a socket is to
      be created.
  @type: Specifies the type of socket to be created.
  @protocol: Specifies a particular protocol to be used with the
      socket. Specifying a protocol of 0 causes socket() to use an
      unspecified default protocol appropriate for the requested
      socket type.
*/
int lothars__socket(int family, int type, int protocol)
{
	int res;
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  The socketpair() function shall create an unbound pair of connected
  sockets in a specified domain, of a specified type, under the
  protocol optionally specified by the protocol argument. The two
  sockets shall be identical. The file descriptors used in referencing
  the created sockets shall be returned in fd[0] and fd[1].

  #include <sys/socket.h>

  @family: The specified domain of the sockets.
  @type: The specified type of the sockets.
  @protocol: The optionally specified protocol for the sockets.
  @fd: The resulting pair of socket descriptors.
*/
void lothars__socketpair(int family, int type, int protocol, int fd[2])
{
	if (0 > socketpair(family, type, protocol, fd)) {
		err_sys("%s() error", __func__);
	}
}







// ---

/* TODO Stevens recommends the following for HP-UX
// in case fix Stevens lib setup for me in config.h - HP-UX
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

/* // BSD - Stevens implementation for BSD, where there is a kqueue() available..
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
// */ // BSD



