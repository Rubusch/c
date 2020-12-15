// client.c
/*
  UDP echo client
//*/

#include "../lib_socket/lib_socket.h"


int main(int argc, char** argv)
{
  int fd_sock;
  struct sockaddr_in servaddr;

  // init server address structure
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);

  // connect the client to the server on ip = 10.0.2.2
  _inet_pton(AF_INET, "10.0.2.2", &servaddr.sin_addr);

  // get the socket descriptor
  fd_sock = _socket(AF_INET, SOCK_DGRAM, 0);

  // connect the socket to the command line console
  dg_cli(stdin, fd_sock, (SA*) &servaddr, sizeof(servaddr));
  
  exit(0);
}

