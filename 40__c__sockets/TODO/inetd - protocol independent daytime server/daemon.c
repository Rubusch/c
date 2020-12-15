// daemon.c
/*
  example of a inetd daemon implementation 

  protocol independent daytime server invoked by inetd (daemon)
//*/

#include "../lib_socket/lib_socket.h"
#include <time.h> // main

#define MAXFD 64


// in error.c
extern int daemon_proc;


/*
  main - tcp daytime server invoked by inetd
//*/
int main(int argc, char** argv)
{
  socklen_t len;
  struct sockaddr *cliaddr;
  char buff[MAXLINE];
  time_t ticks;

  // init daemon with program name
  daemon_inetd(argv[0], 0);

  cliaddr = _malloc(sizeof(struct sockaddr_storage));

  len = sizeof(struct sockaddr_storage);
  _getpeername(0, cliaddr, &len);
  err_msg("connection from %s", _sock_ntop(cliaddr, len));
  
  ticks = time(NULL);  
  snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
  _write(0, buff, strlen(buff));

  // close TCP connection
  _close(0);
  exit(0);
}


