/*
  sockets API

  references:
  - Unix Network Programming, Stevens, 2004
  - The Linux Programming Interface, Kerrisk, 2010



  FAQ:
  1. In a multithreaded server, do socket need mutex protection?

  Generally no...
  -> sockets are full duplex
  -> write is an atomic operation

  If messages are split and the order, etc is not otherwise protected
  (sequence number, resent, timeout, ACK), a series of write syscalls
  will need mutex protection


  stream sockets

  send a bytestream, analogy would be a company running a telephone service


  datagram sockets

  send datagrams, analogy would be a postal office, sending/delivering letters
*/


/*
  Berkley Sockets - NOTES



  socket       family/purpose    packet

  PF_INET      SOCK_STREAM       TCP (Layer 4) -> IP (Layer 3)

  PF_INET      SOCK_DGRAM        UDP (Layer 4) -> IP (Layer 3)

  PF_INET      SOCK_RAW          Raw Sockets (Layer 3)


  PF_PACKET    device            (Layer 2)


  PF_NETLINK   NETLINK_ROUTE

  PF_NETLINK   NETLINK_SKIP

  PF_NETLINK   NETLINK_USERSOCK

  PF_NETLINK   NETLINK_FIREWALL

  PF_NETLINK   NETLINK_ARPD

  PF_NETLINK   NETLINK_ROUTE6

  PF_NETLINK   NETLINK_IP6_FW

  PF_NETLINK   NETLINK_DNRTMSG

  PF_NETLINK   NETLINK_TAPBASE



  CLIENT

  1. socket()

  2. bind() // can - no must!

  3. connect() - only client

  4. write()    | read()       - syscalls
     send()     | recv()       - elaborated
     sendto()   | recvfrom()   - connection oriented
     writev()   | readv()      - TODO    
     sendmsg()  | recvmsg()    - TODO    

  5. close()



  SERVER

  1. socket()

  2. bind()

  3. listen() - only server

  4. accept() - only server

  5. write()    | read()
     send()     | recv()
     sendto()   | recvfrom()
     writev()   | readv()
     sendmsg()  | recvmsg()

  6. close()
*/



// STRUCTS

/*
  struct sockaddr
*/
#include <sys/types.h>
#include <sys/socket.h>
struct sockaddr {
	sa_family_t sa_family; // address family: AF_XXX
	char sa_data[14];     // protocol address (IP and port number)
};
/*
  type for the socket address

  -> old version, complicated for IP and port number:
  sockaddr_in
*/



/*
  struct sockaddr_in
*/
#include <sys/types.h>
#include <sys/socket.h>
struct sockaddr_in {
	sa_family sin_family; // address family: AF_XXX
	u_short sin_port;     // port number, in network byte order
		              //   (little & big endian problem!)
	struct in_addr sin_addr; // IP-Address
	unsigned char pad[8]; // fill byte for the sockaddr
};
/*
  newer struct than sockaddr, commonly used sockaddr_in

  be aware of big/little endian values when using values for "port"
  with more than one byte
*/



/*
  struct iovec
*/
#include <sys/types.h>
#include <sys/socket.h>
struct iovec {
	void *iov_base;       // starting address
	size_t iov_len;       // number of bytes
};
/*
  array references to a memory area
*/



/*
  struct servent
*/
#include <netdb.h>
struct servent {
	char *s_name;         // official server name
	char **s_aliases;     // alias list or NULL
	int s_port;           // port number to the specified server
	char *s_proto;        // protocol name
};
/*
  server address structure
*/



/*
  struct hostent
*/
#include <netdb.h>
struct hostent {
	char *h_name;         // official host name
	char **h_aliases;     // alias list or NULL
	short h_addrtype;     // address type, AF_XXX for IPv4
	short h_length;       // length of the numeric address
	char **h_addr_list;   // array of pointers to addresses
                              //   for the corresponding computers
};
/*
  host address structure
*/





// FUNCTIONS

/*
  socket()
*/
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
/*
  int domain                  // Address family:
      AF_UNIX        UNIX Domain Sockets; local interprocess communication
      AF_INET        Internet IP-Protocol version 4 (IPv4)
      AF_INET6       Internet IP-Portocol version 6 (IPv6)
      AF_IRDA        IRDA-Sockets, e.g. infra red
      AF_BLUETOOTH   Bluetooth-Sockets
      ...
  int type                    // socket type
      SOCK_STREAM    Stream Socket - TCP
      SOCK_DGRAM     Datagram Socket - UDP
      SOCK_RAW       Raw Socket
      ...
  int protocol                // lots of options
                              // and some errors, see
                              // "man 2 socket"

  protocol - protocolname (in case of SOCK_STREAM & SOCK_DGRAM a 0
  provides the settings for the commonly used TCP or UDP)
*/

// Example: socket()
int s;
s = socket(AF_INET, SOCK_STREAM, 0);
if (s == -1) {
	perror("socket() failed");
	return 1;
}
// */



/*
  connect()
*/
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrLen);
/*
  int sockfd                  // socket descriptor
  struct sockaddr *addr       // pointer to the target structur
                              //   to connect the socket
  socklen_t addrLen           // length of the address
                              //   in byte (int?)

  errors:
    see man pages - lots...
*/

// Example: connect()
int s;
struct sockaddr_in addr;
// ... e.g. s = socket(...);
addr.sin_addr.s_addr = ...; // e.g. using inet_pton();
addr.sin_port = ...; // e.g. htons(80);
addr.sin_family = AF_INET;
if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
	perror("connect() failed");
	return 2;
}
// */


/*
  bind()
 */
#include <sys/types.h>
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrLen);
/*
  int sockfd                  // socket descriptor
  struct sockaddr* my_addr    // address to connect to
  socklen_t addrLen           // length of the address

  errors:
    EACCES                    // address is protected
    EADDRINUSE                // address already in use
    EBADF                     // sockfd not a valid socket descriptor
    EINVAL                    // socket is already bound to an address
    ENOTSOCK                  // sockfd is a descriptor fo a file, not a socket
*/

// Example: bind()
int s;
struct sockaddr_in addr;
// ...                        // s = socket(...);
addr.sin_addr.s_addr = INADDR_ANY; // e.g. inet_pton();
addr.sin_port = htons(27976);
addr.sin_family = AF_INET;
if (-1 == (bind(hSocket, (struct sockaddr *)&addr, sizeof(addr)))) {
	perror("bind() failed");
	return 2;
}
// */



/*
  listen()
*/
#include <sys/types.h>
#include <sys/socket.h>
int listen(int sockfd, int backlog);
/*
  int sockfd                  // socket descriptor
  int backlog                 // maximum number of possible
                              //   tries to connect, 5 is common

  errors:
    EADDRINUSE                // another socket already listening
                              //   on same port
    EBADF                     // socket descriptor invalid
    ENOTSOCK                  // socket is not a socket
    EOPNOTSUPP                // socket doesn't support listen()
*/

// Example: listen()
if (listen(s, 3) == -1) {
	perror("listen() failed");
	return 3;
}
// */



/*
  accept()
*/
#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int accept(int socket, struct sockaddr *peer, socklen_t *addrLen);
/*
  int socket                  // socket descriptor
  struct sockaddr* peer       // pointer on the already allocated
                              //   memory to store the other address
  socklen_t* addrLen          // length of the allocated memory


  errors:
    EAGAIN / EWOULDBLOCK      // socket is marked non-blocking and no
                              //   connections present

    EBADF                     // descriptor invalid

    ECONNABORTED              // connection aborted

    EINTR                     // system call interrupted by signal
                              //   caught before

    EINVAL                    // socket is not listening for
                              //   connections

    EMFILE                    // system limit of open file
  			      //   descriptors has been reached

    ENFILE                    // system limit
                              //   of open files has been reached

    ENOTSOCK	              // descriptor
                              //   references a file not a socket

    EOPNOTSUPP                // socket is not of type SOCK_STREAM

    EPAULT (fails)            // addr not writeable part of user
                              //   address space

    ENOBUFS, ENOMEM (fails)   // not enough free memory

    EPROTO (fails)            // protocol error

    EPERM  (fails)            // firewall rules forbid connection

  accept() is blocking the server until the connection has been established
  it is possible to change the behaviour to nonblocking
*/

// Example: accept()
struct sockaddr_in cli;
socklen_t cli_size;
cli_size = sizeof(cli);
if ((c = accept(s, (struct sockaddr *)&cli, &cli_size)) == -1) {
  /* in case of no automatically restarting systemcalls
     if(errno == EINTR) ...
  */
	perror("accept() failed");
	return 4;
}
// */



/*
  select()
 */
#include <sys/types.h>
#include <sys/select.h>       // according to POSIX.1-2001 or
#include <sys/time.h>         // according to earlier standards
#include <unistd.h>

void FD_CLR(int fd, fd_set *set); // This macro removes the file
  // descriptor fd from set. Removing a file descriptor that is not
  // present in the set is a no-op, and does not produce an error.

int FD_ISSET(int fd, fd_set *set); // select() _modifies_ the contents
  // of the sets according to the rules described below. After calling
  // select(), the FD_ISSET() macro can be used to test if a file
  // descriptor is still present in a set. FD_ISSET() returns nonzero
  // if the file descriptor fd is present in set, and zero if it is
  // not.

void FD_SET(int fd, fd_set *set); // This macro adds the file
  // descriptor fd to set. adding a file descriptor that is already
  // present in the set is a no-op, and does not produce an error.

void FD_ZERO(fd_set *set);    // This macro clears (removes all file
  // descriptors from) set. It should be employed as the first step in
  // initializing a file descriptor set.

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
	   struct timeval *timeout);

  // select() with additional signal handling
#define _POSIX_C_SOURCE 200112L
#include <sys/select.h>
int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
	    const struct timespec *timeout, const sigset_t *sigmask);

/*
  IO multiplexing, wait/block on FD to become "ready" or timeout, can
  handle up to a limited amount of FDs;
  timing at non-blocking sockets: basically to avoid blocking

  int nfds                    // highest numbered file descriptor
                              //   plus one
  fd_set* readfds             // read file descriptor
  fd_set* writefds            // write file descriptor
  fd_set* exceptfds           // exeception file descriptor (exceptions)
  struct timeval* timeout     // timeout (secs and msecs)

  errors:
    EBADF                     // file descriptor invalid or already closed
    EINTR                     // was interrupted by a signal
    EINVAL                    // file descriptor negativ or timeout invalid
    ENOMEM                    // unable to allocate memory


  select() - pselect()

  - select() uses a timeout that is a struct timeval (with seconds and
    microseconds), while pselect() uses a struct timespec (with
    seconds and nanoseconds).

  - select() may update the timeout argument to indicate how much time
    was left.  pselect() does not change this argument.

  - select() has no sigmask argument, and behaves as pselect() called
    with NULL sigmask.



  NB: IO multiplexing

  i.e. waiting on a set of file descriptors (FD) to become ready,
  block or timed, then start serving routine;

  IO multiplexing can be implemented by

  - select(): select() & poll() use syscall poll, most portable since
              oldest solution, select() and poll() have +/- same
              performance, are limited to a max number of FDs to be
              dealt with, the API of poll() is more flexible

  - poll(): see select()

  - pselect(): select() additionally signal handling, to avoid the
              situation of race conditions between signal handling and
              FD handling

  - signal-driven IO: better performance for large sets of FDs; uses
              SIGIO and fcntl() approach

  - epoll(): better performance, most flexible, but linux specific
             (i.e. not portable among Unix, since not POSIX
             standardized)



  - asynch IO: needs kernel support, (?)
*/

// Example: select()
#include <sstdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
int main(void)
{
	fd_set rdfs;
	struct timeval tv;
	int retval;

	// watch stdin (fd 0) to see when it has input
	FD_ZERO(&rfds);
	FD_set(0, &rfds);

	// wait up to five seconds
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	retval = select(1, &rfds, NULL, NULL, &tv);
	// don't rely on the value of tv now!

	if (retval == -1)
		perror("select()");
	else if (retval)
		printf("Data is available now.\n");
	else
		printf("No data within five seconds.\n");

	return 0;
}
// */

/*
  Example: sleep using select()

  on systems where there is no sleep() available, sleep can be
  implemented using select()
*/
struct timeval tv;
tv.tv_sec = 0;
tv.tv_usec = 35000; // 35000 microsecs - 35 millisecs
select(0, NULL, NULL, NULL, &tv);
// */



/*
  read()
*/
#include <sys/types.h>
#include <sys/socket.h>
size_t read(int sockfd, void *data, size_t data_len);
/*
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes

  sends to the other socket without flags, only when connected!
*/



/*
  write()
*/
#include <sys/types.h>
#include <sys/socket.h>
size_t write(int sockfd, void *data, size_t data_len);
/*
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes

  receives from the other socket
  without flags, only when connected!
*/



/*
  send()
*/
#include <sys/types.h>
#include <sys/socket.h>
ssize_t send(int sockfd, const void *data, size_t data_len, int flags);
/*
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes
  int flags                   // transmission control

  sends formated to the other socket that already is connected
  if flags is 0, behaves like read() - only when connected!
*/

// Example: send()
int send_banner(int s)
{
	int bytes;
	char banner[] = "Welcome to the Foobar server!\r\n";
	bytes = send(s, banner, strlen(banner), 0);
	if (bytes == -1) {
		perror("send() failed");
		return 1;
	}
	return 0;
}
// */



/*
  recv()
*/
#include <sys/types.h>
#include <sys/socket.h>
dsize_t recv(int sockfd, void *data, size_t data_len, int flags);
/*
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes
  int flags                   // transmission control

  errors:
    some - man 2 recv

  receives formated from the other socket that already is connected
  if flags is 0, behaves like write() - only when connected!
*/

// Example: recv()
#define BUF_SIZ 4096
// ...
int get_banner(int s)
{
	char buf[BUF_SIZ];
	int bytes;
	bytes = recv(s, buf, sizeof(buf) - 1, 0);
	if (bytes == -1) {
		perror("recv() failed");
		return 1;
	}
	buf[bytes] = '\0';
	printf("Banner is \"%s\"\n", buf);
	return 0;
}
// */



/*
  sendto()
*/
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *data, size_t data_len, int flags,
	       struct sockaddr *destaddr, int destLen);
/*
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes
  int flags                   // transmission controll
  struct sockaddr* destaddr   // pointer to sockaddr
  int destLen                 // length of the destination

  errors:
    see "man 2 send"

  sends to a socket FD that needs to be specified in the function
  sends to the net - allways possible
*/

// Example: sendto()
struct sockaddr_in addr;
int s;
char text[] = "Hello World!\r\n";
s = ...; // socket() e.g. using SOCK_DGRAM
inet_pton(AF_INET, "192.168.1.1", &addr.sin_addr.sin_addr);
addr.sin_family = AF_INET;
addr.sin_port = htons(4711);
if (sendto(s, text, strlen(text), 0, (struct sockaddr *)&addr, sizeof(addr)) ==
    -1) {
	perror("sendto() failed");
	return 1;
}
// */



/*
  recvfrom()
*/
#include <sys/types.h>
#include <sys/socket.h>
size_t recvfrom(int sockfd, void *data, size_t data_len, unsigned int flags,
		struct sockaddr *fromaddr, int addrlen);
/*
  int sockfd                  // socket descriptor
  void* data                  // data to send
  size_t data_len             // length of data in bytes
  unsigned int flags          // transmission controll
  struct sockaddr* fromaddr   // pointer to sockaddr
  int addrlen                 // length of the destination

  errors:
    some - man 2 recvfrom

  receives from another socket that has to be specified in the
  function receives from the net - allways possible
*/

// Example: recvfrom()
#define BUF_SIZ 4049
//...
int get_banner_from_localhost(int s)
{
	char buf[BUF_SIZ];
	int bytes;
	struct sockaddr_in addr;
	bytes = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&addr,
			 sizeof(addr));
	if (bytes = -1) {
		perror("recvfrom() failed");
		return 1;
	}
	buf[bytes] = '\0';
	printf("Server sent \"%s\"\n", buf);
	return 0;
}
// */


/*
  readev()
*/
#include <sys/types.h>
#include <sys/socket.h>
int readv(int sockfd, const struct iovec *vector, size_t count);
/*
  int sockfd                  // socket descriptor
  sonst struct iovec* vector  // vector of data to send
  size_t count                // size of that vector

           
*/



/*
  writev()
*/
#include <sys/types.h>
#include <sys/socket.h>
int writev(int sockfd, const struct iovec *vector, size_t count);
/*
  int sockfd                  // socket descriptor
  sonst struct iovec* vector  // vector of data to send
  size_t count                // size of that vector

            
*/



/*
  sendmsg()
*/
#include <sys/types.h>
#include <sys/socket.h>
int sendmsg(int sockfd, const struct msghdr *msg, int flags);
/*
  int sockfd                  // socket descriptor
  const struct msghdr* msg    // message header
  int flags                   // transmission control

              
*/



/*
  recvmsg()
*/
#include <sys/types.h>
#include <sys/socket.h>
int recvmsg(int sockfd, struct msghdr *msg, int flags);
/*
  int sockfd                  // socket descriptor
  const struct msghdr* msg    // message header
  int flags                   // transmission control

                
*/



/*
  close()
*/
#include <unistd.h>
int close(int fd);
/*
  int sockfd                  // socket (file) descriptor

  errors:
    EBADF                     // fd is not a open file descriptor
    EINTR                     // was interrupted by a signal
    EIO                       // io error occurred

  closes the socket
*/

// Example: close()
close(s);
// */



/*
  shutdown()
*/
#include <sys/socket.h>
int shutdown(int s, int how);
/*
  int s                       // socket descriptor
  int how                     // shutdown mode


  values for how:
    SHUT_RD                   // further receptions dissallowed
    SHUT_WR                   // further transmissions dissallowed
    SHUT_RDWR                 // further receptions and
                              // transmissions disallowed

  errors:
    EBADF                     // s is not a valid descriptor
    ENOTCONN                  // the specified socket is not connected
    ENOTSOCK                  // s is a file, not a socket


  NB:
  shutdown() closes the stream, not the FD - close closes the FDs
  altogether; shutdown() can be used to close just the read or just
  write stream, and keep the other open
*/

// Example: shutdown()
shutdown(s, SHUT_WR);
// */



/*
  htons() - host to network short [byteorder]
*/
#include <netinet/in.h>
unsigned short int htons(unsigned short int hostshort);
/*
  unsigned short int hostshort // host-byte-order

  return:  network-byte-order
*/



/*
  htonl() - host to network long [byteorder]
*/
#include <netinet/in.h>
unsigned long int htonl(unsigned long int hostlong);
/*
  unsigned long int hostlong  // host-byte-order

  return:  network-byte-order
*/

/*
  Example: htonl()
  Assignment by the server
*/
struct sockaddr_in server;
memset(&server, 0, sizeof(server));
// IPv4
server.sin_family = AF_INET;
// each IP-Adress is valid
server.sin_addr.s_addr = htonl(INADDR_ANY);
// Portnumber 1234
server.sin_port = htons(1234);
if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
	// handle error at bind()
}




/*
  network to host short [byteorder]
*/
#include <netinet/in.h>
unsigned short int ntohs(unsigned short int netshort);
/*
  unsigned short int netshort // network-byte-order

  return:  host-byte-order
*/



/*
  ntohl() - network to host long [byteorder]
*/
#include <netinet/in.h>
unsigned long int ntohl(unsigned long int netlong);
/*
  unsigned long int netlong   // network-byte-order

  return:  host-byte-order
*/



/*
  getsockname()
*/
#include <sys/types.h>
#include <sys/socket.h>
int getsockname(int s, struct sockaddr *name, socklen_t *namelen);
/*
  int s                       // socket descriptor
  struct sockaddr* name       // name of the socket to return
  socklen_t* namelen          // length of the name

  errors:
    EBADF                     // not a valid socket descriptor
    EFAULT                    // *name points to invalid memory
    EINVAL                    // namelen is invalid (e.g. negative)
    ENOBUFS                   // insufficient resources available in
                              //   the system
    ENOTSOCK                  // s is a file, not a socket
*/

// Example: getsockname()
struct sockaddr_in addr;
sockelnt_t len;
// ... // s = socket ....
len = sizeof(addr);
getsockname(s, (struct sockaddr *)&addr, &len);
// */



/*
  getpeername()
*/
#include <sys/types.h>
#include <sys/socket.h>
int getpeername(int s, struct sockaddr *name, socklen_t *namelen);
/*
  int s                       // socket descriptor
  struct sockaddr* name       // name of the socket to return
  socklen_t* namelen          // length of the name

  errors:
    EBADF                     // not a valid socket descriptor
    EFAULT                    // *name points to invalid memory
    EINVAL                    // namelen is invalid (e.g. negative)
    ENOBUFS                   // insufficient resources available
                              //   in the system
    ENOTCONN                  // socket is not connected
    ENOTSOCK                  // s is a file, not a socket
*/

// Example: getpeername()
struct sockaddr_in addr;
socklen_t len;
// ...
len = sizeof(addr);
getpeername(s, (struct sockaddr *)&addr, &len);
// */



/*
  getaddrinfo()
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *node, const char *service,
		const struct addrinfo *hints,
		struct addrinfo **res);
/*
  given a hostname and/or a service name, getaddrinfo() returns a set
  of strucutres containing the corresponding binary IP address(es) and
  port number

  getaddrinfo() may accept either a node or a service where the other
  may be NULL

  NB: getaddrinfo() allocates a struct addrinfo, to be free'd by
  freeaddrinfo()


  prefer getaddrinfo(), avoid old replacements!

  DEPRECATED:
  - gethostbyname()
  - getservbyname()
*/



/*
  freeaddrinfo()
*/
void freeaddrinfo(struct addrinfo *res);
/*
  TODO
*/



/*
  getnameinfo()
*/
#include <sys/socket.h>
#include <netdb.h>
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
		char *host, socklen_t hostlen,
		char *serv, socklen_t servlen, int flags);
/*
  converts a socket address to a corresponding host and service, in a
  protocol-independent manner

  DEPRECATED:
  - gethostbyaddr()
  - getservbyport()
*/



/*
  getsockopt() - get socket options
*/
#include <sys/types.h>
#include <sys/socket.h>
int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
/*
  int s                       // socket descriptor
  int level                   // SOL_SOCKET level
  int optname                 // name of a option
  void* optval                // set option or buffer for a address
  socklen_t *optlen           // set option or buffer

  errors:
    EBADF                     // no valid socket descriptor
    EFAULT                    // address in optval invalid
    ENOPROTOOPT               // option unknown at the socket level
    ENOTSOCK                  // s is a file not a socket
*/



/*
  setsockopt() - set socket options
*/
#include <sys/types.h>
#include <sys/socket.h>
int setsockopt(int s, int level, int optname, const void *optval,
	       socklen_t optlen);
/*
  int s                       // socket descriptor
  int level                   // SOL_SOCKET level
  int optname                 // name of a option
  void* optval                // set option or buffer for a address
  socklen_t *optlen           // set option or buffer

  errors:
    EBADF                     // no valid socket descriptor
    EFAULT                    // address in optval invalid
    EINVAL                    // optlen invalid
    ENOPROTOOPT               // option unknown at the socket level
    ENOTSOCK                  // s is a file not a socket
*/



/*
  inet_pton() - address conversion from text to binary form
                (protocol independent)
*/
#include <arpa/inet.h>
int inet_pton(int af, const char *src, void *dst);
/*
  const char* cp              // internet host address to binary
                              //   data and stores it in
  struct in_addr* pin         // ...this address structure


  use inet_pton() - avoid using old replacements!

  DEPRECATED:
  - inet_aton() and
  - inet_addr()
*/



/*
  inet_ntop() - address conversion from binary form to text
                (protocol dependent)
*/
#include <arpa/inet.h>
const char *inet_ntop(int af, const void *src,
                             char *dst, socklen_t size);
/*
  avoid using old replacements!

  DEPRECATED:
  - inet_ntoa()
*/



/*
  fcntl()
*/
// TODO         

/*
  Example: fcntl()
  not blocking sockets
*/
#ifdef WIN32 // win
unsigned long ul = 1;
ioctlsocket(s, FIONBIO, &ul);
#else
fcntl(s, F_SETFL, O_NONBLOCK);
#endif



/*
  Errors:

  use of errno() and threadsafety, in case use _r co-named functions for
  asynch-signal-safe and/or thread-safe implementations

  a function is not threadsafe if it returns pointer to memory, uses
  globals or statics
*/
