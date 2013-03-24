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



  CLIENT:

  1. socket()

  2. bind() // can - no must!

  3. connect() - only client

  4. write()    | read()
     send()     | recv()
     sendto()   | recvfrom()
     writev()   | readv()
     sendmsg()  | recvmsg()

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
//*/



// STRUCTS

#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
struct sockaddr{
  sa_family_t sa_family;              // address family: AF_XXX
  char sa_data[14];                   // protocol address (IP and port number)
};
/*
  sockaddr - type for the socket address

  -> old version, complicated for IP and port number:
  sockaddr_in
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
struct sockaddr_in{
  sa_family sin_family;               // address family: AF_XXX
  unsigned short int sin_port;        // port number, in network byte order 
                                      // (little & big endian problem!)
  struct in_addr sin_addr;            // IP-Address
  unsigned char pad[8];               // fill byte for the sockaddr
};
/*  
  newer struct than sockaddr, commonly used sockaddr_in

  be aware of big/little endian values when using values for "port" with more than one byte
//*/
// Example
  char* szServerIP = argv[1];
  unsigned int serverPort = 27976;
  printf("0. set up client for server at:\'%s\' on port \'%i\'\n", szServerIP, serverPort);

  int hSocket = 0;
  struct sockaddr_in server_addr;
  struct hostent *host;
  if(!inet_aton(szServerIP, &server_addr.sin_addr)){
    if(!(host = gethostbyname(szServerIP))){
      perror("gethostbyname() failed");
      return 2;
    }
    server_addr.sin_addr = *(struct in_addr*) host->h_addr;
  }
  server_addr.sin_port = htons(serverPort);
  server_addr.sin_family = AF_INET;



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
struct iovec{
  void* iov_base;                     // starting address
  size_t iov_len;                     // number of bytes
};
/*
  array references to a memory area
//*/



#include <netdb.h>
struct servent{
  char *s_name;                       // official server name
  char **s_aliases                    // alias list or NULL
  int s_port;                         // port number to the specified server
  char *s_proto;                      // protocol name
};
/*
  server address structure
//*/



#include <netdb.h>
struct hostent{
  char *h_name;                       // official host name
  char **h_aliases;                   // alias list or NULL
  short h_addrtype;                   // address type, AF_XXX for IPv4
  short h_length;                     // length of the numeric address
  char **h_addr_list;                 // array of pointers to addresses for the corresponding computers
};
/*
  host address structure
//*/




// FUNCTIONS

#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
/*
  int domain                          // Address family:
      AF_UNIX        UNIX Domain Sockets; local interprocess communication
      AF_INET        Internet IP-Protocol version 4 (IPv4)
      AF_INET6       Internet IP-Portocol version 6 (IPv6)
      AF_IRDA        IRDA-Sockets, e.g. infra red
      AF_BLUETOOTH   Bluetooth-Sockets
      ...
  int type                             // socket type
      SOCK_STREAM    Stream Socket - TCP
      SOCK_DGRAM     Datagram Socket - UDP
      SOCK_RAW       Raw Socket
      ...
  int protocol                         // lots of options and some errors, see "man 2 socket"

  protocol - protocolname (in case of SOCK_STREAM & SOCK_DGRAM a 0 
  provides the settings for the commonly used TCP or UDP)
//*/
// Example - socket():
int s;
s = socket(AF_INET, SOCK_STREAM, 0);
if(s == -1){
  perror("socket() failed");
  return 1;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int connect( int sockfd, const struct sockaddr* addr, socklen_t addrLen);
/*
  int sockfd                          // socket descriptor
  struct sockaddr *addr               // pointer to the target structur to connect the socket
  socklen_t addrLen                   // length of the address in byte (int?)

  errors:
  see man pages - lots...
//*/
// Example:
int s;
struct sockaddr_in addr;
// ... e.g. s = socket(...);
addr.sin_addr.s_addr = ...;           // e.g. inet_addr("127.0.0.1");
addr.sin_port = ...;                  // e.g. htons(80);
addr.sin_family = AF_INET;
if(connect(s, (struct sockaddr*) &addr, sizeof(addr)) == -1){
  perror("connect() failed");
  return 2;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int bind( int sockfd, const struct sockaddr* my_addr, socklen_t addrLen);
/*
  int sockfd                          // socket descriptor
  struct sockaddr* my_addr            // address to connect to
  socklen_t addrLen                   // length of the address

  errors: TODO
  EACCES                              // address is protected
  EADDRINUSE                          // address already in use
  EBADF                               // sockfd not a valid socket descriptor
  EINVAL                              // socket is already bound to an address
  ENOTSOCK                            // sockfd is a descriptor fo a file, not a socket
//*/
// Example:
int s;
struct sockaddr_in addr;
// ...                                // s = socket(...);
addr.sin_addr.s_addr = INADDR_ANY;           // e.g. inet_addr("127.0.0.1");
addr.sin_port = htons(27976);       
addr.sin_family = AF_INET;
if(-1 == (bind(hSocket, (struct sockaddr*) &addr, sizeof(addr)))){
  perror("bind() failed");
  return 2;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int listen(int sockfd, int backlog);
/*
  int sockfd                          // socket descriptor
  int backlog                         // maximum number of possible tries to connect, 5 is common

  errors:
  EADDRINUSE                          // another socket already listening on same port
  EBADF                               // socket descriptor invalid
  ENOTSOCK                            // socket is not a socket
  EOPNOTSUPP                          // socket doesn't support listen()
//*/
// Example:
if(listen(s, 3) == -1){
  perror("listen() failed");
  return 3;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int accept(int socket, struct sockaddr* peer, socklen_t* addrLen);
/*
  int socket                          // socket descriptor
  struct sockaddr* peer               // pointer on the already allocated memory to store the other address
  socklen_t* addrLen                  // length of the allocated memory

  errors:
  EAGAIN / EWOULDBLOCK                // socket is marked non-blocking and no connections present
  EBADF                               // descriptor invalid
  ECONNABORTED                        // connection aborted
  EINTR                               // system call interrupted by signal caught before
  EINVAL                              // socket is not listening for connections
  EMFILE                              // system limit of open file descriptors has been reached
  ENFILE                              // system limit of open files has been reached
  ENOTSOCK                            // descriptor references a file not a socket
  EOPNOTSUPP                          // socket is not of type SOCK_STREAM
  EPAULT (fails)                      // addr not writeable part of user address space
  ENOBUFS, ENOMEM (fails)             // not enough free memory
  EPROTO (fails)                      // protocol error
  EPERM (fails)                       // firewall rules forbid connection
  
  accept() is blocking the server until the connection has been established
  it is possible to change the behaviour to nonblocking
//*/
// Example:
struct sockaddr_in cli;
socklen_t cli_size;
cli_size = sizeof(cli);
if((c = accept(s, (struct sockaddr*) &cli, &cli_size)) == -1){
  /* in case of no automatically restarting systemcalls
     if(errno == EINTR) ...
  */
  perror("accept() failed");
  return 4;
}
//*/



#include <sys/select.h> // according to POSIX.1-2001 or
#include <sys/time.h>   // according to earlier standards
#include <sys/types.h>  //
#include <unistd.h>     //
int select(int nfds, fd_set* readfds, fd_set* writefds
	  , fd_set* exceptfds, struct timeval* timeout);
void FD_CLR(int fd, fd_set* set);
int FD_ISSET(int fd, fd_set* set);
void FD_SET(int fd, fd_set* set);
void FD_ZERO(fd_set* set);
#define _XOPEN_SOURCE 600
#include <sys/select.h>
int pselect(int nfds, fd_set* readfds, fd_set* writefds,
	    fd_set* exceptfds, const struct timespec* timeout,
	    const sigset_t* sigmask);
/*
  int nfds                            // highest numbered file descriptor plus one
  fd_set* readfds                     // read file descriptor
  fd_set* writefds                    // write file descriptor
  fd_set* exceptfds                   // exeception file descriptor (exceptions)
  struct timeval* timeout             // timeout (secs and msecs)
  struct timespec* timeout            // timeout (secs and nsecs)
  struct sigset_t*sigmask             // sigmask

  errors:
  EBADF                               // file descriptor invalid or already closed
  EINTR                               // was interrupted by a signal
  EINVAL                              // file descriptor negativ or timeout invalid
  ENOMEM                              // unable to allocate memory

  timing at non-blocking sockets - basically to avoid blocking

  - select() uses a timeout that is a struct timeval (sec & msecs)
  pselect() uses a struct timespec( secs and nsecs)
  - select() may update the timeout - pselect() not
  - select() has no sigmask (behaves like pselect() with NULL for sigmask
//*/
// Example
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

  if(retval == -1)
    perror("select()");
  else if(retval)
    printf("Data is available now.\n");
  else
    printf("No data within five seconds.\n");

  return 0;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
size_t read( int sockfd, void* data, size_t data_len);
/*
  int sockfd                          // socket descriptor
  const void* data                    // data to send 
  size_t data_len                     // length of data in bytes

  sends to the other socket
  without flags, only when connected!
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
size_t write( int sockfd, void* data, size_t data_len);
/*
  int sockfd                          // socket descriptor
  const void* data                    // data to send 
  size_t data_len                     // length of data in bytes

  receives from the other socket
  without flags, only when connected!
//*/
// Example:
// TODO



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
ssize_t send( int sockfd, const void* data, size_t data_len, int flags);
/*
  int sockfd                          // socket descriptor
  const void* data                    // data to send 
  size_t data_len                     // length of data in bytes
  int flags                           // transmission control

  sends formated to the other socket that already is connected
  if flags is 0, behaves like read() - only when connected!
//*/
// Example:
int send_banner(int s)
{
  int bytes;
  char banner[] = "Welcome to the Foobar server!\r\n";
  bytes = send(s, banner, strlen(banner), 0);
  if(bytes == -1){
    perror("send() failed");
    return 1;
  }
  return 0;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
dsize_t recv( int sockfd, void* data, size_t data_len, int flags);
/*
  int sockfd                          // socket descriptor
  const void* data                    // data to send 
  size_t data_len                     // length of data in bytes
  int flags                           // transmission control

  errors:
  some - man 2 recv

  receives formated from the other socket that already is connected
  if flags is 0, behaves like write() - only when connected!
//*/
// Example:
#define BUF_SIZ 4096
// ...
int get_banner(int s)
{
  char buf[BUF_SIZ];
  int bytes;
  bytes = recv(s, buf, sizeof(buf) - 1, 0);
  if(bytes == -1){
    perror("recv() failed");
    return 1;
  }
  buf[bytes] = '\0';
  printf("Banner is \"%s\"\n", buf);
  return 0;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
ssize_t sendto( int sockfd, const void* data, 
		size_t data_len, int flags, 
		struct sockaddr* destaddr, int destLen);
/*
  int sockfd                          // socket descriptor
  const void* data                    // data to send 
  size_t data_len                     // length of data in bytes
  int flags                           // transmission controll
  struct sockaddr* destaddr           // pointer to sockaddr
  int destLen                         // length of the destination

  errors:
  see "man 2 send"

  sends to a socket that needs to be specified in the function
  sends to the net - allways possible
//*/
// Example:
struct sockaddr_in addr;
int s;
char text[] = "Hello World!\r\n";
s = ...; // socket() e.g. using SOCK_DGRAM
addr.sin_addr.sin_addr = inet_addr("192.168.1.1");
addr.sin_family = AF_INET;
addr.sin_port = htons(4711);
if(sendto(s, text, strlen(text), 0, (struct sockaddr*) &addr, sizeof(addr)) == -1){
  perror("sendto() failed");
  return 1;
}
//*/


#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
size_t recvfrom( int sockfd, void* data, size_t data_len, unsigned int flags, struct sockaddr* fromaddr, int addrlen);
/*
  int sockfd                          // socket descriptor
  void* data                          // data to send 
  size_t data_len                     // length of data in bytes
  unsigned int flags                  // transmission controll
  struct sockaddr* fromaddr           // pointer to sockaddr
  int addrlen                         // length of the destination

  errors:
  some - man 2 recvfrom

  receives from another socket that has to be specified in the function
  receives from the net - allways possible
//*/
// Example:
#define BUF_SIZ 4049
//...
int get_banner_from_localhost(int s)
{
  char buf[BUF_SIZ];
  int bytes;
  struct sockaddr_in addr;
  bytes = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr*) &addr, sizeof(addr));
  if(bytes = -1){
    perror("recvfrom() failed");
    return 1;
  }
  buf[bytes] = '\0';
  printf("Server sent \"%s\"\n", buf);
  return 0;
}
//*/



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int readv(int sockfd, sonst struct iovec* vector, size_t count);
/*
  int sockfd                          // socket descriptor
  sonst struct iovec* vector          // vector of data to send
  size_t count                        // size of that vector
//*/
// Example:
// TODO



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int writev(int sockfd, const struct iovec* vector, size_t count);
/*
  int sockfd                          // socket descriptor
  sonst struct iovec* vector          // vector of data to send
  size_t count                        // size of that vector
//*/
// Example:
// TODO



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int sendmsg(int sockfd, const struct msghdr* msg, int flags);
/*
  int sockfd                          // socket descriptor
  const struct msghdr* msg            // message header
  int flags                           // transmission control
//*/
// Example:
// TODO



#include <sys/types.h> // before <sys/socket.h> !!!
#include <sys/socket.h>
int recvmsg(int sockfd, struct msghdr* msg, int flags);
/*
  int sockfd                          // socket descriptor
  const struct msghdr* msg            // message header
  int flags                           // transmission control
//*/
// Example:
// TODO



#include <unistd.h>
int close(int fd);
/*
  int sockfd                          // socket (file) descriptor

  errors:
  EBADF                               // fd is not a open file descriptor
  EINTR                               // was interrupted by a signal
  EIO                                 // io error occurred

  closes the socket
//*/
// Example:
close(s);
//*/



#include <sys/socket.h>
int shutdown(int s, int how);
/*
  int s                                // socket descriptor
  int how                              // shutdown mode 


  values for how:
  SHUT_RD                              // further receptions dissallowed
  SHUT_WR                              // further transmissions dissallowed
  SHUT_RDWR                            // further receptions and transmissions disallowed

  
  errors:
  EBADF                                // s is not a valid descriptor
  ENOTCONN                             // the specified socket is not connected
  ENOTSOCK                             // s is a file, not a socket
//*/
// Example
shutdown(s, SHUT_WR);
//*/



#include <netinet/in.h>
unsigned short int htons(unsigned short int hostshort);
/*
  unsigned short int hostshort         // host-byte-order

  return:  network-byte-order
//*/



#include <netinet/in.h>
unsigned long int htonl(unsigned long int hostlong);
/*
  unsigned long int hostlong           // host-byte-order

  return:  network-byte-order
//*/



#include <netinet/in.h>
unsigned short int ntohs(unsigned short int netshort);
/*
  unsigned short int netshort          // network-byte-order

  return:  host-byte-order
//*/



#include <netinet/in.h>
unsigned long int ntohl(unsigned long int netlong);
/*
  unsigned long int netlong            // network-byte-order

  return:  host-byte-order
//*/ 



#include <netdb.h>
struct servent* getservbyname(const char* name, const char* proto);
/*
  const char* name                     // servername to look up data
  const char* proto                    // protocol

  looks up the port number to a specified server, uses the following struct
  and returns NULL in case of a failure.
  
  errors:
  returns NULL if the buffer supplied by caller is not long enough (ERANGE)

//*/
// Example:
struct servernt* serv;
serv = getservbyname("ftp", "tcp");
if(!serv){
  perror("getservbyname() failed");
  return 1;
}
printf("serv->s_port = %u\n", ntohs(serv->s_port));
//*/



#include <netdb.h>
struct servent* getservbyport(int port, const char* proto);
/*
  int port                             // port number
  const char* proto                    // protocol, e.g. "tcp" or "udp"

  errors:
  -
//*/
// Example:
struct servent* serv;
serv = getservbyport(htons(80), "tcp");
if(!serv){
  perror("getservbyname() failed");
  return 1;
}
printf("serv->s_name = %\n", serv->s_name);
//*/



#include <sys/types.h>
#include <sys/socket.h>
int getsockname(int s, struct sockaddr* name, socklen_t* namelen);
/*
  int s                                // socket descriptor 
  struct sockaddr* name                // name of the socket to return 
  socklen_t* namelen                   // length of the name

  errors:
  EBADF                                // not a valid socket descriptor
  EFAULT                               // *name points to invalid memory
  EINVAL                               // namelen is invalid (e.g. negative)
  ENOBUFS                              // insufficient resources available in the system
  ENOTSOCK                             // s is a file, not a socket
//*/
// Example:
struct sockaddr_in addr;
sockelnt_t len;
// ... // s = socket ....
len = sizeof(addr);
getsockname(s, (struct sockaddr*) &addr, &len);
//*/



#include <sys/types.h>
#include <sys/socket.h>
int getpeername(int s, struct sockaddr* name, socklen_t* namelen);
/*
  int s                                // socket descriptor 
  struct sockaddr* name                // name of the socket to return 
  socklen_t* namelen                   // length of the name

  errors:
  EBADF                                // not a valid socket descriptor
  EFAULT                               // *name points to invalid memory
  EINVAL                               // namelen is invalid (e.g. negative)
  ENOBUFS                              // insufficient resources available in the system
  ENOTCONN                             // socket is not connected
  ENOTSOCK                             // s is a file, not a socket
//*/
// Example:
struct sockaddr_in addr;
socklen_t len;
// ...
len = sizeof(addr);
getpeername(s, (struct sockaddr*) &addr, &len);
//*/



#include <netdb.h>
struct hostent* gethostbyname(const char* name);
/* 
  const char* name                     // hostname to look up data
  const char* proto                    // protcol
   
  resolve host look up by port number, uses the following struct
  and returns NULL in case of a failure.
   
  errors:
  HOST_NOT_FOUND                       // host is unknown
  NO_ADDRESS, NO_DATA                  // name is valid but does not have an IP 
  NO_RECOVERY                          // a non-recoverable name server error occurred
  TRY_AGAIN                            // temp error on name server - try again later
//*/
struct sockaddr_in addr;
struct hostent *host;
// ...
host = gethostbyname("www.freebsd.org");
if(!host){
  herror("gethostbyname() failed");
  return 1;
}
// ...
addr.sin_addr = *(struct in_addr*) host->h_addr;
// ...
//*/



#include <netdb.h>
extern int h_errno;
struct hostent* gethostbyaddr(const char* addr, int len, int type);
/*
  const char* addr                     // address of requested host  
  int len                              // length of address
  int type                             // type of the address

  errors:
  HOST_NOT_FOUND                       // host is unknown
  NO_ADDRESS, NO_DATA                  // name is valid but does not have an IP 
  NO_RECOVERY                          // a non-recoverable name server error occurred
  TRY_AGAIN                            // temp error on name server - try again later
//*/
// Example
struct in_addr addr;
struct hostent* host;
addr.s_addr = inet_addr("127.0.0.1");
host = gethostbyaddr((char*) &addr, sizeof(addr), AF_INET);
if(!host){
  herror("gethostbyaddr() failed");
  return 1;
}
printf("host->h_name = %s\n", host->h_name);
//*/



#include <sys/types.h>
#include <sys/socket.h>
int getsockopt(int s, int level, int optname, void* optval, socklen_t *optlen);
/*
  int s                                // socket descriptor
  int level                            // SOL_SOCKET level
  int optname                          // name of a option
  void* optval                         // set option or buffer for a address
  socklen_t *optlen                    // set option or buffer

  errors:
  EBADF                                // no valid socket descriptor
  EFAULT                               // address in optval invalid
  ENOPROTOOPT                          // option unknown at the socket level
  ENOTSOCK                             // s is a file not a socket
//*/



#include <sys/types.h>
#include <sys/socket.h>
int setsockopt(int s, int level, int optname, const void* optval, socklen_t optlen);
/*
  int s                                // socket descriptor
  int level                            // SOL_SOCKET level
  int optname                          // name of a option
  void* optval                         // set option or buffer for a address
  socklen_t *optlen                    // set option or buffer

  errors:
  EBADF                                // no valid socket descriptor
  EFAULT                               // address in optval invalid
  EINVAL                               // optlen invalid
  ENOPROTOOPT                          // option unknown at the socket level
  ENOTSOCK                             // s is a file not a socket
//*/



#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int inet_aton(const char* cp, struct in_addr* pin);
/*
  const char* cp                       // internet host address to binary data and stores it in
  struct in_addr* pin                  // ...this address structure
//*/



#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
in_addr_t inet_addr(const char* cp);
/*
  const char* cp                       // string to internet standard dot notation
//*/



#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
char* inet_ntoa(struct in_addr in);
/*
  struct in_addr in                    // internet host address to string
//*/





// EXAMPLES
/*
  - send strings using ssprintf() or snprintf() as char* and 
  - be aware of bigger numbers (big endian/little endian prob)
  - take care of the buffer size (between 512 and 1024 bytes)
  - IPv4 or IPv6: in6_addr, sockaddr_in6, getaddrinfo(), getnameinfo(),
  getipnodebyname(), geteipnodebyaddr()
//*/




/*
  Example: 
  filling up connect() using the above functions (TCP),
  for UDP no connect is necessary, but sendto() and recvfrom():
//*/
struct sockaddr_in server;
unsigned long addr;
// ...
// Alternative to memset() -> bzero() ;-)
memset(&server, 0, sizeof(server));
addr = inet_addr(argv[1]);
memcpy( (char*) &server.sin_addr, &addr, sizeof(addr));
server.sin_family = AF_INET;
server.sin_port = htons(80);
// ...
// establish a connection to the server
if(connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0){
  // handle error at connect()
}




/*
  Example: Assignment by the server
//*/
struct sockaddr_in server;
memset( &server, 0, sizeof(server));
// IPv4
server.sin_family = AF_INET;
// each IP-Adress is valid
server.sin_addr.s_addr = htonl( INADDR_ANY);
// Portnumber 1234
server.sin_port = htons( 1234);
if(bind( sock, (struct sockaddr*) &server, sizeof(server)) < 0){
  // handle error at bind()
}




/*
  Example: main loop - server
//*/
stuct sockaddr_in client;
int sock, sock2;
socklen_t len;
for(;;){
  len - sizeof(client);
  sock2 = accept(sock, (struct sockaddr*) &client, &len);
  if(sock2 < 0){
    // handle error at accept()
  }
  // data exchange here
}



/*
  sleep using select()
//*/
struct timeval tv;
tv.tv_sec = 0;
tv.tv_usec = 35000; // 35000 microsecs - 35 millisecs
select(0, NULL, NULL, NULL, &tv);



/*
  not blocking sockets
//*/
#ifdef WIN32 // win
unsigned long ul = 1;
ioctlsocket(s, FIONBIO, &ul);
#else
fcntl(s, F_SETFL, O_NONBLOCK);
#endif



/*
  Errors:

  use errno() ???
//*/
