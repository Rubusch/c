#include "lib_read_write.h"

/*
  The read() function shall attempt to read nbyte bytes from the file
  associated with the open file descriptor, fildes, into the buffer
  pointed to by buf. The behavior of multiple concurrent reads on the
  same pipe, FIFO, or terminal device is unspecified.

  #include <unistd.h>

  @fd: The file descriptor to read from (fildes).
  @ptr: A pointer to the buffer to read into (buf).
  @nbytes: The number of bytes to read.

  Return is phony for compatibility at failure, else returns the
  number of bytes actually read and may be smaller or equal to nbytes.
*/
ssize_t lothars__read(int fd, void *ptr, size_t nbytes)
{
	ssize_t bytes;
	if (-1 == (bytes = read(fd, ptr, nbytes))) {
		err_sys("read error");
	}
	return bytes;
}

/*
  readline_fd

  The original readline() will read a line from the terminal and
  return it, using prompt as a prompt. If prompt is NULL or the empty
  string, no prompt is issued. The line returned is allocated with
  malloc(3); the caller must free it when finished. The line returned
  has the final newline removed, so only the text of the line remains.

  The readline_fd() reads via file descriptor, while the lib
  readline() does not take a file descriptor. It is implemented by the
  use of read(), thus there is no allocation with malloc()
  happening. Buffer and size must be provided by arguments.

  #include <stdio.h>
  #include <readline/readline.h>
  #include <readline/history.h>

  @fd: The file descriptor to read from.
  @ptr: A pointer to the buffer to read into.
  @maxlen: The number of bytes to read.

  Returns the number of bytes actually read (less than or equal
  maxlen), or exits the program in case of failure.
*/
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];
static ssize_t readline_fd_read(int fd, char *ptr)
{
	if (0 >= read_cnt) {
	again:
		if (0 > (read_cnt = read(fd, read_buf, sizeof(read_buf)))) {
			if (errno == EINTR) {
				goto again;
			}
			return -1;
		} else if (read_cnt == 0) {
			return 0;
		}
		read_ptr = read_buf;
	}

	--read_cnt;
	*ptr = *read_ptr++;
	return 1;
}
ssize_t readline_fd(int fd, void *vptr, size_t maxlen)
{
	ssize_t cnt, rc;
	char chr, *ptr;

	ptr = vptr;
	for (cnt = 1; cnt < maxlen; ++cnt) {
		if (1 == (rc = readline_fd_read(fd, &chr))) {
			*ptr++ = chr;
			if (chr == '\n') {
				break; // newline is stored, like fgets()
			}

		} else if (rc == 0) {
			*ptr = 0;
			return (cnt - 1); // EOF, n - 1 bytes were read

		} else {
			return -1; // error, errno set by read()
		}
	}

	*ptr = 0; /* null terminate like fgets() */
	return cnt;
}
ssize_t lothars__readline_fd(int fd, void *ptr, size_t maxlen)
{
	ssize_t bytes;
	if (0 > (bytes = readline_fd(fd, ptr, maxlen))) {
		err_sys("readline error");
	}
	return bytes;
}

/*
  The read() function shall attempt to read nbyte bytes from the file
  associated with the open file descriptor, fildes, into the buffer
  pointed to by buf. The behavior of multiple concurrent reads on the
  same pipe, FIFO, or terminal device is unspecified.

  This wrapper reads exactly nbytes.

  #include <unistd.h>

  @fd: The file descriptor to read from (fildes).
  @ptr: A pointer to the buffer to read into (buf).
  @nbytes: The number of bytes to read.

  Return not before number of bytes are read from the stream.
*/
ssize_t readn(int fd, void *vptr, size_t nbytes)
{
	size_t nleft;
	ssize_t nread;
	char *ptr = NULL;

	ptr = vptr;
	nleft = nbytes;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				nread = 0; // and call read() again
			} else {
				return -1;
			}
		} else if (nread == 0) {
			break; // EOF
		}
		nleft -= nread;
		ptr += nread;
	}
	return (nbytes - nleft); // return >= 0
}
ssize_t lothars__readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t res;
	if (0 > (res = readn(fd, ptr, nbytes))) {
		err_sys("readn error");
	}
	return res;
}
/*
  readn() - approach of The Linux Programming Interface, M.Kerrisk,
  2010, p. 1255
* /
ssize_t
readn(int fd, void *vptr, size_t nbytes)
{
	ssize_t nread;  // # bytes fetched by last read()
	size_t total_read;  // total # of bytes read so far
	char *buf;

	buf = vptr;  // no pointer writhmetic on "void*"
	for (total_read = 0; total_read < nbytes; ) {
		nread = read(fd, buf, nbytes - total_read);
		if (0 == nread) {  // EOF
			return total_read;  // may be 0 if this is first read()
		}
		if (-1 == nread) {
			if (EINTR == errno) {
				continue;  // interrupted --> restart read()
			} else {
				return -1;  // some other error
			}
		}
		total_read += nread;
		buf += nread;
	}
	return total_read;  // must be 'nbytes' if we get here
}
// */

/*
  The recvfrom() function shall receive a message from a
  connection-mode or connectionless-mode socket. It is normally used
  with connectionless-mode sockets because it permits the application
  to retrieve the source address of received data.

  CMSG_SPACE() returns the number of bytes an ancillary element with
  payload of the passed data length occupies. This is a constant
  expression.

  This wrapper reads via file descriptor.

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor (socket).
  @ptr: Points to the buffer where the message should be stored
      (buffer).
  @nbytes: Specifies the length in bytes of the buffer pointed to by
      the buffer argument (length).
  @recvfd: The received file descriptor to read from.

  Returns the number of bytes read.
*/
// TODO verify that cmsghdr is working and remove marked sections
ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t cnt;

	//#ifdef HAVE_MSGHDR_MSG_CONTROL // TODO rm, struct cmsghdr should be available under Linux
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr = NULL;
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	//#else // TODO rm, struct cmsghdr should be available under Linux
	//	int newfd;
	//	msg.msg_accrights = (caddr_t) &newfd;
	//	msg.msg_accrightslen = sizeof(int);
	//#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if (0 >= (cnt = recvmsg(fd, &msg, 0))) {
		return cnt;
	}

	//#ifdef HAVE_MSGHDR_MSG_CONTROL // TODO rm, struct cmsghdr should be available under Linux
	if ((NULL != (cmptr = CMSG_FIRSTHDR(&msg))) &&
	    (cmptr->cmsg_len == CMSG_LEN(sizeof(int)))) {
		if (cmptr->cmsg_level != SOL_SOCKET) {
			err_quit("control level != SOL_SOCKET");
		}
		if (cmptr->cmsg_type != SCM_RIGHTS) {
			err_quit("control type != SCM_RIGHTS");
		}
		*recvfd = *((int *)CMSG_DATA(cmptr));

	} else {
		*recvfd = -1; // descriptor was not passed
	}
	//#else // TODO rm, struct cmsghdr should be available under Linux
	//	if (msg.msg_accrightslen == sizeof(int)) {
	//		*recvfd = newfd;
	//	} else {
	//		*recvfd = -1;  // descriptor was not passed
	//	}
	//#endif

	return cnt;
}
ssize_t lothars__read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	ssize_t cnt;
	if (0 > (cnt = read_fd(fd, ptr, nbytes, recvfd))) {
		err_sys("read_fd error");
	}
	return cnt;
}

/*
  The write() function shall attempt to write nbyte bytes from the
  buffer pointed to by buf to the file associated with the open file
  descriptor, fildes.

  #include <unistd.h>

  @fd: The associated open file descriptor.
  @ptr: Points to the buffer to write to.
  @nbytes: The number of bytes to write.
*/
void lothars__write(int fd, void *ptr, size_t nbytes)
{
	if (nbytes != write(fd, ptr, nbytes)) {
		err_sys("write error");
	}
}

/*
  The write() function shall attempt to write nbyte bytes from the
  buffer pointed to by buf to the file associated with the open file
  descriptor, fildes.

  This wrapper writes exactly number of bytes to the descriptor.

  #include <unistd.h>

  @fd: The associated open file descriptor.
  @ptr: Points to the buffer to write to.
  @nbytes: The number of bytes to write.
*/
ssize_t writen(int fd, const void *vptr, size_t num)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = num;
	while (0 < nleft) {
		if (0 >= (nwritten = write(fd, ptr, nleft))) {
			if ((nwritten < 0) && (errno == EINTR)) {
				// and call write() again
				nwritten = 0;
			} else {
				// error
				return -1;
			}
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return num;
}
void lothars__writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes) {
		err_sys("writen error");
	}
}
/*
  writen() - approach of The Linux Programming Interface, M.Kerrisk,
  2010, p. 1255
* /
ssize_t
writen(int fd, const void *vptr, size_t nbytes)
{
	ssize_t nwritten;  // # of bytes written by last write()
	size_t total_written;   // total # of bytes written so far
	const char* buf;

	buf = vptr;  // no pointer arithmetic on "void*"
	for (total_written = 0; total_written < nbytes; ) {
		nwritten = write(fd, buf, nbytes - total_written);
		if (0 >= nwritten) {
			if (nwritten == -1 && EINTR == errno) {
				continue;  // interruppted --> restart write()
			} else {
				return -1;  // some other error
			}
		}
		total_written += nwritten;
		buf += nwritten;
	}
	return total_written;  // must be 'nbytes' if we get here
}
// */

/*
  The sendmsg() function shall send a message through a
  connection-mode or connectionless-mode socket. If the socket is
  connectionless-mode, the message shall be sent to the address
  specified by msghdr. If the socket is connection-mode, the
  destination address in msghdr shall be ignored.

  CMSG_DATA() returns a pointer to the data portion of a cmsghdr.

  This wrapper writes on a file descriptor.

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor (socket).
  @ptr: Points to the buffer where the message should be stored
      (buffer).
  @nbytes: Specifies the length in bytes of the buffer pointed to by
      the buffer argument (length).
  @sendfd: The sending file descriptor to write to.

  Returns number of bytes written, or stops the program.
*/
// TODO investigate HAVE_MSGHDR_MSG_CONTROL
ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	struct msghdr msg;
	struct iovec iov[1];

	//#ifdef HAVE_MSGHDR_MSG_CONTROL // TODO rm, after verification
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *)CMSG_DATA(cmptr)) = sendfd;
	//#else // TODO rm, after verification
	//	msg.msg_accrights = (caddr_t) &sendfd;
	//	msg.msg_accrightslen = sizeof(int);
	//#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return sendmsg(fd, &msg, 0);
}
ssize_t lothars__write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	ssize_t bytes;
	if (0 > (bytes = write_fd(fd, ptr, nbytes, sendfd))) {
		err_sys("write_fd error");
	}
	return bytes;
}
