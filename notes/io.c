// read / write

/*
  read()
*/
#include <sys/types.h>
#include <sys/socket.h>
size_t read(int sockfd, void *data, size_t data_len);
/*
  Parameters:
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes

  Returns...
  - If no process has the pipe open for writing, read() shall return 0
    to indicate end-of-file.
  - If O_NONBLOCK is set, read() shall return -1 and set errno to
    [EAGAIN].

  Syscall - Attempts to read nbyte bytes from the file associated with
  the open file descriptor, only when connected!
*/



/*
  write()
*/
#include <sys/types.h>
#include <sys/socket.h>
size_t write(int sockfd, void *data, size_t data_len);
/*
  Parameters:
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes

  Returns...
  Upon successful completion, these functions shall return the number
  of bytes actually written to the file associated with.
  - If write() is interrupted by a signal before it writes any data,
    it shall return -1 with errno set to [EINTR].
  - If write() is interrupted by a signal after it successfully writes
    some data, it shall return the number of bytes written.

  Syscall - Attempts to write nbyte bytes from the buffer pointed to
  by buf to the file associated with the open file descriptor, only
  when connected!
*/



// readev() / writev()

/*
  readev()
*/
#include <sys/types.h>
#include <sys/socket.h>
int readv(int sockfd, const struct iovec *vector, size_t count);
/*
  Parameters:
  int sockfd                  // socket descriptor
  sonst struct iovec* vector  // vector of data to send
  size_t count                // size of that vector

  scatter-gather-read...
  Instead of accepting a single buffer of data to be read or written,
  these functions transfer multiple buffers of data in a single system
  call.

  The linux Programming Interface, Kerrisk, 2010, p.99
*/



/*
  writev()
*/
#include <sys/types.h>
#include <sys/socket.h>
int writev(int sockfd, const struct iovec *vector, size_t count);
/*
  Parameters:
  int sockfd                  // socket descriptor
  sonst struct iovec* vector  // vector of data to send
  size_t count                // size of that vector

  Instead of accepting a single buffer of data to be read or written,
  these functions transfer multiple buffers of data in a single system
  call.

  The linux Programming Interface, Kerrisk, 2010, p.99
*/



// pread() / pwrite()

/*
  pread()
*/
#include <unistd.h>
ssize_t pread(int fd, void *buf, size_t count, off_t offset);
/*
  Parameters:
  int fd                      // file descriptor
  void *buf                   // read into buf
  size_t count                // number of bytes to read
  off_t offset                // offset, from where to read

  Returns a number of bytes read, 0 on EOF or -1 on error


  pread() reads up to count bytes from file descriptor fd at offset
  offset (from the start of the file) into the buffer starting at
  buf. The file offset is not changed.

  The pread() and pwrite() system calls operate just like read() and
  write(), except that the file I/O is performed at the location
  specified by offset, rather than at the current file offset. The
  file offset is left unchanged by these calls.

  Using pread() and pwrite(), multiple threads can simultaneously
  perform I/O on the same file descriptor (FD) without being affected
  by changes made to the file offset by other threads.

  The linux Programming Interface, Kerrisk, 2010, p.98
*/



/*
  pwrite()
*/
#include <unistd.h>
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
/*
  Parameters:
  int fd                      // file descriptor
  void *buf                   // from where to write
  size_t count                // number of bytes to write
  off_t offset                // offset in file, to start writing

  Returns a number of bytes written, or -1 on error


  pwrite() writes up to count bytes from the buffer starting at buf to
  the file descriptor fd at offset offset. The file offset is not
  changed.

  The pread() and pwrite() system calls operate just like read() and
  write(), except that the file I/O is performed at the location
  specified by offset, rather than at the current file offset. The
  file offset is left unchanged by these calls.

  Using pread() and pwrite(), multiple threads can simultaneously
  perform I/O on the same file descriptor (FD) without being affected
  by changes made to the file offset by other threads.

  The linux Programming Interface, Kerrisk, 2010, p.98
*/



// recv() / send()

/*
  recv()
*/
#include <sys/types.h>
#include <sys/socket.h>
dsize_t recv(int sockfd, void *data, size_t data_len, int flags);
/*
  Parameters:
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes
  int flags                   // transmission control

  The return value and the first three arguments to recv() and send()
  are the same as for read() and write().

  errors:
    some - man 2 recv

  Receives formated from the other socket that already is connected
  if flags is 0, behaves like write() - only when connected!

  The recv() and send() system calls perform I/O on connected
  sockets. They provide socket-specific functionality that is not
  available with the traditional read() and write() system calls.

  The linux Programming Interface, Kerrisk, 2010, p.1259
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
  send()
*/
#include <sys/types.h>
#include <sys/socket.h>
ssize_t send(int sockfd, const void *data, size_t data_len, int flags);
/*
  Parameters:
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes
  int flags                   // transmission control

  Sends formated to the other socket that already is connected
  if flags is 0, behaves like read() - only when connected!

  Receives formated from the other socket that already is connected
  if flags is 0, behaves like write() - only when connected!

  The recv() and send() system calls perform I/O on connected
  sockets. They provide socket-specific functionality that is not
  available with the traditional read() and write() system calls.

  The linux Programming Interface, Kerrisk, 2010, p.1259
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



// recvfrom() / sendto()

/*
  recvfrom()
*/
#include <sys/types.h>
#include <sys/socket.h>
size_t recvfrom(int sockfd, void *data, size_t data_len, unsigned int flags,
		struct sockaddr *fromaddr, int addrlen);
/*
  Parameters:
  int sockfd                  // socket descriptor
  void* data                  // data to send
  size_t data_len             // length of data in bytes
  unsigned int flags          // transmission controll
  struct sockaddr* fromaddr   // pointer to sockaddr
  int addrlen                 // length of the destination

  Returns number of bytes received, 0 on EOF, or -1 on error

  errors:
    some - man 2 recvfrom

  The recvform() and sendto() system calls receive and send datagrams
  on a datagram socket. The socket does not need to be connected
  (datagram = connectionless)

  The linux Programming Interface, Kerrisk, 2010, p.1161
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
  sendto()
*/
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *data, size_t data_len, int flags,
	       struct sockaddr *destaddr, int destLen);
/*
  Parameters:
  int sockfd                  // socket descriptor
  const void* data            // data to send
  size_t data_len             // length of data in bytes
  int flags                   // transmission controll
  struct sockaddr* destaddr   // pointer to sockaddr
  int destLen                 // length of the destination

  Returns number of bytes sent, or -1 on error

  errors:
    see "man 2 send"

  The recvform() and sendto() system calls receive and send datagrams
  on a datagram socket. The socket does not need to be connected
  (datagram = connectionless)

  The linux Programming Interface, Kerrisk, 2010, p.1161
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



// recvmsg() / sendmsg()

/*
  recvmsg()
*/
#include <sys/types.h>
#include <sys/socket.h>
int recvmsg(int sockfd, struct msghdr *msg, int flags);
/*
  Parameters:
  int sockfd                  // socket descriptor
  const struct msghdr* msg    // message header
  int flags                   // transmission control

  Returns...
  Upon successful completion, recvmsg() shall return the length of the
  message in bytes, -1 on error


  The recvmsg() function shall receive a message from a
  connection-mode or connectionless-mode socket. It is normally used
  with connectionless-mode sockets because it permits the application
  to retrieve the source address of received data.

  The sendmsg() and recvmsg() system calls are the most general
  purpose of the socket I/O system calls. The sendmsg() system call
  can do everything that is done by write(), send() and sendto(); the
  recvmsg() system call can do everything that is done by read recv()
  and recvfrom().

  In addition:
  - perform scatter-gather I/O as with readv() and
    writev()
  - transmit messages containing domain-specific anciallary
    data (also known as control information).

  The linux Programming Interface, Kerrisk, 2010, p.1284
*/



/*
  sendmsg()
*/
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
/*
  Parameters:
  int sockfd                  // socket descriptor
  const struct msghdr* msg    // message header
  int flags                   // transmission control

  Returns...
  Upon successful completion, sendmsg() shall return the number of
  bytes sent. Otherwise, -1


  The sendmsg() and recvmsg() system calls are the most general
  purpose of the socket I/O system calls. The sendmsg() system call
  can do everything that is done by write(), send() and sendto(); the
  recvmsg() system call can do everything that is done by read recv()
  and recvfrom().

  In addition:
  - perform scatter-gather I/O as with readv() and
    writev()
  - transmit messages containing domain-specific anciallary
    data (also known as control information).

  NB: for sending / receiveing multiple datagrams:
  - sendmmsg()
  - recvmmsg()

  The linux Programming Interface, Kerrisk, 2010, p.1284
*/



/*
  sendfile()
*/
#include <sys/sendfile.h>
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
/*
  Parameters:
  int out_fd                  // the FD opened for writing
  int in_fd                   // the FD opened for reading
  off_t *offset               // offset from where to start reading, or NULL
                              //   (the file offset)
  size_t count                // number of bytes to copy between the
                              //   file descriptors

  Returns number of bytes transferred, or -1 on error

  sendfile() copies data between one file descriptor and another.
  Because this copying is done within the kernel, sendfile() is more
  efficient than the combination of read(2) and write(2), which would
  require transferring data to and from user space.

  The sendfile() system call transfers bytes from the file referred to
  by the descriptor in_fd to the file referred to by the descriptor
  out_fd. The out_fd descriptor must refer to a socket. The in_fd
  argument must refer to a file to which mmap() can be applied; in
  practice this usually means a regular file. This somewhat restricts
  the use of sendfile(). We can use it to pass data from a file to a
  socket, but not vice versa.

  The linux Programming Interface, Kerrisk, 2010, p.1261
*/
