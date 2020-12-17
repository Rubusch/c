// client.c
/*
  tcp server types - TCP testing client for all server types
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* close() */

#include <stdarg.h>  // ANSI C
//#include <syslog.h>  // syslog
#include <errno.h>

// TODO rm       
//#include "../lib_socket/lib_socket.h"       


// max number of bytes to request from server
#define MAXN 1234


// forwards
pid_t _fork();
ssize_t _read(int, void *, size_t);
void _write(int, void *, size_t);
void _close(int);

static void err_doit(int, int, const char *, va_list);

/*
   Miscellaneous constants
*/
#define MAXLINE  4096                // max text line length
#define BUFFSIZE 8192                // buffer size for reads and writes


// internal helpers

/*
   Fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
  va_list  ap;
  va_start(ap, fmt);
  err_doit(1, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(1);
}

/*
   Fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
  va_list  ap;
  va_start(ap, fmt);
  err_doit(0, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(1);
}

/*
   Print message and return to caller Caller specifies "errnoflag" and "level"
*/
static void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
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

  if(daemon_proc){
    syslog(level, buf);
  }else{
    fflush(stdout);  // in case stdout and stderr are the same
    fputs(buf, stderr);
    fflush(stderr);
  }

  return;
}

// adjusted functions
pid_t _fork(void)
{
  pid_t pid;

  if(-1 == (pid = fork())){
    err_sys("fork error");
  }
  return(pid);
}

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

void _write(int fd, void *ptr, size_t nbytes)
{
  if(nbytes != write(fd, ptr, nbytes)){
    err_sys("write error");
  }
}

void _close(int fd)
{
  if(-1 == close(fd)){
    err_sys("close error");
  }
}



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
  for(idx=0; idx<n_children; ++idx){

    // if is not parent!
    if(0 == (pid = _fork())){

      // per loop...
      for(jdx=0; jdx<n_loops; ++jdx){

        // connect to 10.0.2.2 via port 27976
        fd_connect = _tcp_connect("10.0.2.2", "27976");

        // write to socket
        _write(fd_connect, request, strlen(request));

        // read response
        if(n_bytes != (bytes = _readn(fd_connect, reply, n_bytes))){
          err_quit("server returned %d bytes", bytes);
        }

        // close socket
        _close(fd_connect);
      }
      printf("child %d done\n", idx);
      exit(0);
    }

  } // parent loops around to fork() again

  while(0 < wait(NULL))
    ;

  if(errno != ECHILD){
    err_sys("wait error");
  }

  exit(0);
}
