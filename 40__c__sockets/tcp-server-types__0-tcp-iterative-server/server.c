// server.c
/*
  TCP iteractive server (no process control, faster is impossible!)

  An iterative TCP server proceses each client's request completely 
  before moving on to the next client. Iterative TCP serers are rare!
//*/

#include "../lib_socket/lib_socket.h"
#include <time.h>


/*
//*/
int main(int argc, char** argv)
{
  int fd_listen, fd_conn;
  struct sockaddr_in servaddr;
  char buf[MAXLINE];
  time_t ticks;
  
  // set up listen socket
  fd_listen = _socket(AF_INET, SOCK_STREAM, 0);

  // set up server address
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(27976); // connect to port 27976

  // bind "listen socket" to the "server address"
  _bind(fd_listen, (SA*) &servaddr, sizeof(servaddr));

  // listen on socket (queue length == LISTENQ == 1024)
  _listen(fd_listen, LISTENQ);

  // server loop
  while(1){
    // accept (blocking!) - ...listen socket to connection socket
    fd_conn = _accept(fd_listen, (SA*) NULL, NULL);
    
    // example action: send a string containing the system time
    ticks = time(NULL);
    snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
    fprintf(stdout, "sending \"%s\"\n", buf);
    _write(fd_conn, buf, strlen(buf));
    
    // close connection after sending
    _close(fd_conn);
  }
}
