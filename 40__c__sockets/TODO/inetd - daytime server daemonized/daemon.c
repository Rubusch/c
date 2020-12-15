// daemon.c
/*
  example of a inetd daemon implementation 
  invoking a simple daytime server
//*/

#include "../lib_socket/lib_socket.h"
#include <syslog.h> // init
#include <time.h> // main

#define MAXFD 64


// in error.c
extern int daemon_proc;

/*
  init - snippet (contained in lib_socket)

int daemon_init(const char* pname, int facility)
{
  int idx;
  pid_t pid;
  
  // fork
  if(0 > (pid = _fork())){
    return -1;
  }else if(pid){
    // parent
    _exit(0);
  }

  // child 1 continues
  if(setsid() < 0){
    return -1;
  }

  _signal(SIGHUP, SIG_IGN);
  if(0 > (pid = _fork())){
    return -1;
  }else if(pid){
    // child1 (==parent 2)
    exit(0);
  }

  // child 2 continues
  // for err_xxx() functions
  daemon_proc = 1;

  // change working directory
  chdir("/");
  
  // close off file descriptors
  for(idx=0; idx<MAXFD; ++idx){
    close(idx);
  }

  // redirect stdin, stdout, and stderr to /dev/null
  open("/dev/null", O_RDONLY);
  open("/dev/null", O_RDWR);
  open("/dev/null", O_RDWR);

  openlog(pname, LOG_PID, facility);
  
  // success
  return 0;
}
//*/


/*
  main - tcp daytime server invoked by inetd
//*/
int main(int argc, char** argv)
{
  int fd_listen, fd_conn;
  socklen_t addrlen, len;
  struct sockaddr *cliaddr;
  char buff[MAXLINE];
  time_t ticks;

  // init daemon with program name
  daemon_init(argv[0], 0);

  // init socket: <ip>, <port>, &addrlen
  fd_listen = _tcp_listen(NULL, "27976", &addrlen);

  cliaddr = _malloc(addrlen);
  while(1){
    len = addrlen;
    fd_conn = _accept(fd_listen, cliaddr, &len);
    err_msg("connection from %s", _sock_ntop(cliaddr, len));
    
    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    _write(fd_conn, buff, strlen(buff));
    
    _close(fd_conn);
  }
    
  return 0;
}

