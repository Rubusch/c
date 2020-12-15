// mcastsockets.c
/*
  multicast socket - start sender and receiver
//*/

#include "../lib_socket/lib_socket.h"
#include <sys/utsname.h>

// send one dataram every five seconds
#define SENDRATE 5


/*
  send
//*/
void send_all(int fd_send, SA* sadest, socklen_t salen)
{
  char line[MAXLINE];
  struct utsname name;
  
  if(0 > uname(&name)){
    err_sys("uname error");
  }

  snprintf(line, sizeof(line), "%s, %d\n", name.nodename, getpid());
  
  while(1){
    _sendto(fd_send, line, strlen(line), 0, sadest, salen);
    sleep(SENDRATE); // wait sendrate to resend
  }
}


/*
  receive
//*/
void recv_all(int fd_recv, socklen_t salen)
{
  int idx;
  char line[MAXLINE + 1];
  socklen_t len;
  struct sockaddr *safrom;
  
  safrom = _malloc(salen);

  while(1){
    len = salen;
    idx = _recvfrom(fd_recv, line, MAXLINE, 0, safrom, &len);
    
    // '\0' termination
    line[idx] = '\0';
    printf("from %s: %s", _sock_ntop(safrom, len), line);
  }
}


/*
  main...
//*/
int main(int argc, char** argv)
{
  int fd_send, fd_recv;
  const int on = 1;
  socklen_t salen;
  struct sockaddr *sa_send, *sa_recv;

  // change settings <IP-mcast-addr> and <port>
  fd_send = _udp_client("10.0.2.2", "27976", (void*) &sa_send, &salen);
  
  fd_recv = _socket(sa_send->sa_family, SOCK_DGRAM, 0);
  
  _setsockopt(fd_recv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  sa_recv = _malloc(salen);
  memcpy(sa_recv, sa_send, salen);

  _bind(fd_recv, sa_recv, salen);

  _mcast_join(fd_recv, sa_send, salen, NULL, 0);
  _mcast_set_loop(fd_send, 0);
  
  if(_fork() == 0){
    // child - receives
    recv_all(fd_recv, salen);
  }
  // parent - sends
  send_all(fd_send, sa_send, salen);
  
  return 0;
}
