// client.c
/*
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h> // before <sys/socket.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 1024
#define PORT 27976

extern int inet_aton(const char*, struct in_addr*);


int send_message(int hSocket, char message[MESSAGE_SIZE])
{
  // TODO
  return 0;
}


int receive_message(int hSocket, char message[MESSAGE_SIZE])
{
  // TODO
  return 0;
}


int main(int argc, char** argv)
{
  if(argc != 2){
    puts("./Client <xxx.xxx.xxx.xxx>");
    return EXIT_FAILURE;
  }
  printf(" *** Client ***\n"); 
    

  // init
  char* szServerIP = argv[1];

  // 1. socket()
  int hSocket = 0;
  if(-1 == (hSocket = socket(AF_INET, SOCK_STREAM, 0))){
    perror("socket() failed");
    return EXIT_FAILURE;
  }
     

  // connect()
  struct sockaddr_in addrServer;
  struct hostent *hostServer = NULL;
  if(!inet_aton(szServerIP, &addrServer.sin_addr)){
    if(!(hostServer = gethostbyname(szServerIP))){
      perror("gethostbyname() failed");
      return EXIT_FAILURE;
    }
    addrServer.sin_addr = *(struct in_addr*) hostServer->h_addr;
  }
  addrServer.sin_port = PORT;
  addrServer.sin_family = AF_INET;
  printf("connecting to socket to %s: port: %i...\n", 
	 inet_ntoa(addrServer.sin_addr), ntohs(addrServer.sin_port));
  fflush(stdout);
  if(-1 == connect(hSocket, (struct sockaddr*) &addrServer, sizeof(addrServer))){
    perror("connect() failed");
    return EXIT_FAILURE;
  }
  puts("Ok.");


  // send()/recv()
  // TODO

  // close()
  close(hSocket);
      

  return EXIT_SUCCESS;
}
