// client.c
/*
  tcp server types - TCP testing client for all server types
 */

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> /* wait() */
#include <unistd.h> /* read(), close() */
#include <sys/types.h> /* getaddrinfo() */
#include <sys/socket.h> /* getaddrinfo() */
#include <netdb.h> /* getaddrinfo() */
#include <stdarg.h>
#include <errno.h>


/*
  constants
*/
#define MAXLINE  4096 /* max text line length */
#define BUFFSIZE 8192 /* buffer size for reads and writes */
#define MAXN 1234 /* max number of bytes to request from server */


/*
  forwards
*/
ssize_t readn(int, void*, size_t);
static void err_doit(int, const char *, va_list);

pid_t lothars__fork();
int lothars__tcp_connect(const char*, const char*);
ssize_t lothars__readn(int, void *, size_t);
void lothars__write(int, void *, size_t);
void lothars__close(int);




/*
  internal helpers
*/

/*
   print message and return to caller Caller specifies "errnoflag"

   error handling taken from "Unix Sockets" (Stevens)
*/
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
  int errno_save, n_len;
  char buf[MAXLINE + 1];

  errno_save = errno; // value caller might want printed

#ifdef HAVE_VSNPRINTF
  vsnprintf(buf, MAXLINE, fmt, ap); // safe
#else
  vsprintf(buf, fmt, ap); // not safe
#endif

  n_len = strlen(buf);
  if(errnoflag){
    snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
  }

  strcat(buf, "\n");

  fflush(stdout);  // in case stdout and stderr are the same
  fputs(buf, stderr);
  fflush(stderr);

  return;
}


/*
  read "num" bytes from a descriptor
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


/*
  helpers / wrappers

  mainly to cover error handling and display error message
*/

/*
   fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
  va_list  ap;
  va_start(ap, fmt);
  err_doit(1, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}

/*
   fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
  va_list  ap;
  va_start(ap, fmt);
  err_doit(0, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}


pid_t lothars__fork(void)
{
  pid_t pid;

  if(-1 == (pid = fork())){
    err_sys("fork error");
  }

  return(pid);
}


int lothars__tcp_connect(const char *host, const char *serv)
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

    lothars__close(sockfd); // ignore this one
  }while(NULL != (res = res->ai_next));

  if(NULL == res){ // errno set from final connect()
    err_sys("tcp_connect error for %s, %s", host, serv);
  }

  freeaddrinfo(ressave);

  return(sockfd);
}


ssize_t lothars__readn(int fd, void *ptr, size_t nbytes)
{
  ssize_t  res;
  if(0 > (res = readn(fd, ptr, nbytes))){
    err_sys("readn error");
  }
  return res;
}


void lothars__write(int fd, void *ptr, size_t nbytes)
{
  if(nbytes != write(fd, ptr, nbytes)){
    err_sys("write error");
  }
}


void lothars__close(int fd)
{
  if(-1 == close(fd)){
    err_sys("close error");
  }
}


/*
  main()

  starts a bunch of children to connect to a server, sends some bytes
  and waits on answer from the server

  a tcp server is expected on on 10.0.2.2, and listenes on port 27976
*/
int main(int argc, char** argv)
{
  int idx
    , jdx
    , fd_connect
    , n_children
    , n_loops
    , n_bytes;

  pid_t pid;
  ssize_t bytes;
  char request[MAXLINE], reply[MAXN];

  // number of children
  n_children = 7;

  // number of sockets to open, write, read and close again
  n_loops = 20;

  // number of max bytes to read
  n_bytes = 64;

  // init request to send to server - ends with \n
  snprintf(request, sizeof(request), "%d\n", n_bytes);

  // per child...
  for (idx=0; idx<n_children; ++idx) {

    // if is not parent!
    if (0 == (pid = lothars__fork())) {

      // per loop...
      for (jdx=0; jdx<n_loops; ++jdx) {

        // connect to 10.0.2.2 via port 27976
        fd_connect = lothars__tcp_connect("10.0.2.2", "27976");

        // write to socket
        lothars__write(fd_connect, request, strlen(request));

        // read response
        if (n_bytes != (bytes = lothars__readn(fd_connect, reply, n_bytes))) {
          err_quit("server returned %d bytes", bytes);
        }

        // close socket
        lothars__close(fd_connect);
      }
      printf("child %d done\n", idx);
      exit(EXIT_FAILURE);
    }

  } // parent loops around to fork() again

  while(0 < wait(NULL))
    ;

  if(errno != ECHILD){
    err_sys("wait error");
  }

  exit(EXIT_SUCCESS);
}
