// write_fd.c
/*
  sendmsg()

  cmsg; CMSG_SPACE(); CMSG_FIRSTHDR(); CMSG_LEN(); CMSG_DATA();...

  These macros are used to create and access control messages (also called
  ancillary data) that are not a part of the socket payload. This control
  information may include the interface the packet was received on, various
  rarely used header fields, an extended error description, a set of file
  descriptors or UNIX credentials. For instance, control messages can be used to
  send additional header fields such as IP options. Ancillary data is sent by
  calling sendmsg(2) and received by calling recvmsg(2). See their manual pages
  for more information.

  Ancillary data is a sequence of struct cmsghdr structures with appended data.
  This sequence should only be accessed using the macros described in this
  manual page and never directly. See the specific protocol man pages for the
  available control message types. The maximum ancillary buffer size allowed per
  socket can be set using /proc/sys/net/core/optmem_max; see socket(7).
 */

#include <stdlib.h> // exit()
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/socket.h> // msghdr, CMSG_SPACE(), CMSG_FIRSTHDR(), CMSG_LEN()
                        // CMSG_DATA()
//#include <sys/types.h> // TODO


/*
  internally completely POSIX, so returning ssize_t
 */
ssize_t write_fd( int fd, void *ptr, size_t nbytes, int sendfd )
{
/*
  The recvmsg() call uses a msghdr structure to minimize the number of directly
  supplied arguments. This structure is defined as follows in <sys/socket.h>:

  struct iovec {                        // Scatter/gather array items
          void  *iov_base;              // Starting address
          size_t iov_len;               // Number of bytes to transfer
  };

  struct msghdr {
          void         *msg_name;       // optional address
          socklen_t     msg_namelen;    // size of address
          struct iovec *msg_iov;        // scatter/gather array
          size_t        msg_iovlen;     // # elements in msg_iov
          void         *msg_control;    // ancillary data, see below
          size_t        msg_controllen; // ancillary data buffer len
          int           msg_flags;      // flags on received message
  };

  Here msg_name and msg_namelen specify the source address if the socket is
  unconnected; msg_name may be given as a NULL pointer if no names are desired
  or required. The fields msg_iov and msg_iovlen describe scatter-gather
  locations, as discussed in readv(2). The field msg_control, which has length
  msg_controllen, points to a buffer for other protocol control-related messages
  or miscellaneous ancillary data. When recvmsg() is called, msg_controllen
  should contain the length of the available buffer in msg_control; upon return
  from a successful call it will contain the length of the control message
  sequence.

  The messages are of the form:

  struct cmsghdr {
          socklen_t     cmsg_len;     // data byte count, including hdr
          int           cmsg_level;   // originating protocol
          int           cmsg_type;    // protocol-specific type
          // followed by
          unsigned char cmsg_data[]
  };
  Ancillary data should only be accessed by the macros defined in cmsg(3).

  As an example, Linux uses this ancillary data mechanism to pass extended
  errors, IP options, or file descriptors over UNIX domain sockets.

 */
        struct msghdr msg;
        struct iovec iov[1];
//*
// when MSGHDR MSG CONTROL is available (linux, normally)
        union {
                struct cmsghdr cm;
// CMSG_SPACE() returns the number of bytes an ancillary element with payload of
// the passed data length occupies. This is a constant expression.
                char control[CMSG_SPACE( sizeof(int) )];
        } control_un;
        struct cmsghdr *cmptr;

        msg.msg_control = control_un.control;
        msg.msg_controllen = sizeof( control_un.control );

        cmptr = CMSG_FIRSTHDR( &msg ); // TODO
        cmptr->cmsg_len = CMSG_LEN( sizeof(int) );
        cmptr->cmsg_level = SOL_SOCKET;
        cmptr->cmsg_type = SCM_RIGHTS; // TODO

        // TODO
        *((int*) CMSG_DATA( cmptr )) = sendfd;
/*/
        // else, under certain unices MSGHDR MSG CONTROL might not be available
        msg.msg_accrights = (caddr_t) &sendfd;
        msg.msg_accrightslen = sizeof( int );
//*/

        // TODO
        msg.msg_name = NULL;
        msg.msg_namelen = 0;

        // TODO
        iov[0].iov_base = ptr;
        iov[0].iov_len = nbytes;
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;

        // sendmsg
        return sendmsg( fd, &msg, 0);
}


                                                                      
/*
ssize_t
write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
        struct msghdr        msg;
        struct iovec        iov[1];

#ifdef        HAVE_MSGHDR_MSG_CONTROL
        union {
          struct cmsghdr        cm;
          char                                control[CMSG_SPACE(sizeof(int))];
        } control_un;
        struct cmsghdr        *cmptr;

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
// end write_fd

ssize_t
Write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
        ssize_t                n;

        if ( (n = write_fd(fd, ptr, nbytes, sendfd)) < 0)
                err_sys("write_fd error");

        return(n);
}
//*/
