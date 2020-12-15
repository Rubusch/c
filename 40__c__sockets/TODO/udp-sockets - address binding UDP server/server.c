// server.c
/*
  binding interface addresses with udp server
//*/

#include "../lib_socket/lib_ifi/ifi.h"


/*
  helper
//*/
void server_dg_echo(int fd_sock, SA* pcliaddr, socklen_t clilen, SA* serveraddr)
{
  int cnt;
  char mesg[MAXLINE];
  socklen_t len;
  
  while(1){
    len = clilen;
    cnt = _recvfrom(fd_sock, mesg, MAXLINE, 0, pcliaddr, &len);
    printf("child %d, datagram from %s", getpid(), _sock_ntop(pcliaddr, len));
    printf(", to %s\n", _sock_ntop(serveraddr, clilen));

    _sendto(fd_sock, mesg, cnt, 0, pcliaddr, len);
  }
}


/*
  main function to the UDP server (forked)
//*/
int main(int argc, char** argv)
{
  int fd_sock;
  const int on = 1;
  pid_t pid;
  struct ifi_info *ifi, *ifi_head;
  struct sockaddr_in *sa, cliaddr, wildaddr;
  
  for(ifi_head = ifi = _get_ifi_info(AF_INET, 1)
        ; ifi != NULL
        ; ifi = ifi->ifi_next){
    // bind unicast address
    fd_sock = _socket(AF_INET, SOCK_DGRAM, 0);
    _setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    sa = (struct sockaddr_in*) ifi->ifi_addr;
    sa->sin_family = AF_INET;
    sa->sin_port = htons(SERV_PORT);
    _bind(fd_sock, (SA*) sa, sizeof(*sa));
    printf("bound %s\n", _sock_ntop((SA*) sa, sizeof(*sa)));
    
    if(0 == (pid = _fork())){
      // child
      server_dg_echo(fd_sock, (SA*) &cliaddr, sizeof(cliaddr), (SA*) sa);
      exit(0); // never executed
    }

    /*
      If the interface supports broadcasting, a UDP socket is created
      and the broadcast address is bound to it. We allow the bind to fail
      with EADDRINUSE because if an interface has multiple addresses 
      (aliases) on the same broadcast address.
    //*/

    if(ifi->ifi_flags & IFF_BROADCAST){
      // try to bind broadcast address
      fd_sock = _socket(AF_INET, SOCK_DGRAM, 0);
      _setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
      
      sa = (struct sockaddr_in*) ifi->ifi_brdaddr;
      sa->sin_family = AF_INET;
      sa->sin_port = htons(SERV_PORT);
      if(0 > bind(fd_sock, (SA*) sa, sizeof(*sa))){
        if(errno == EADDRINUSE){
          printf("EADDRINUSE: %s\n", _sock_ntop((SA*) sa, sizeof(*sa)));
          _close(fd_sock);
          continue;
        }else{
          err_sys("bind error for %s", _sock_ntop((SA*) sa, sizeof(*sa)));
        }
      }
        
      printf("bound %s\n", _sock_ntop((SA*) sa, sizeof(*sa)));
      
      if(0 == (pid = _fork())){
        // child
        server_dg_echo(fd_sock, (SA*) &cliaddr, sizeof(cliaddr), (SA*) sa);
        exit(0); // never executed
      }
    }
  }

  // bind wildcard address
    
  fd_sock = _socket(AF_INET, SOCK_DGRAM, 0);
  _setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  
  bzero(&wildaddr, sizeof(wildaddr));
  wildaddr.sin_family = AF_INET;
  wildaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  wildaddr.sin_port = htons(SERV_PORT);
  _bind(fd_sock, (SA*) &wildaddr, sizeof(wildaddr));
  
  printf("bound %s\n", _sock_ntop((SA*) &wildaddr, sizeof(wildaddr)));
  if(0 == (pid = _fork())){
    // child
    server_dg_echo(fd_sock, (SA*) &cliaddr, sizeof(cliaddr), (SA*) sa);
    exit(0); // never executed
  }
  exit(0);  
}
