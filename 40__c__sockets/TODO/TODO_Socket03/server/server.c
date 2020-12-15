// server.c
/*
  
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 1024

int receive_message()
{
  // TODO
  return 0;
}


int send_message()
{
  // TODO
  return 0;
}


int main(int argc, char** argv)
{
  puts(" *** Server ***");
  
  // init
  int clientPort=27976;
  int hSocket=0, hClient=0;


  // socket
  if(-1 == (hSocket == socket(AF_INET, SOCK_STREAM, 0))){
    perror("socket() failed");
    return 1;
  }

  // bind
  struct sockaddr_in addrClient;
  addrClient.sin_addr.s_addr = INADDR_ANY;
  addrClient.sin_family = AF_INET;
  addrClient.sin_port = htons(clientPort);
  if(-1 == bind(hSocket, (struct sockaddr*) &addrClient, sizeof(addrClient))){
    perror("bind() failed");
    return EXIT_FAILURE; // FIXME
  }


  // listen
  printf("backlog: %i\n", SOMAXCONN); // XXX
  if(-1 == listen(hSocket, 3)){
    perror("listen() failed");
    return EXIT_FAILURE;
  }


  // loop
  unsigned int run_server = 1;
  while(run_server){

    // accept
    socklen_t addrLenClient = sizeof(addrClient);
    if(-1 == (hClient = accept(hSocket, (struct sockaddr*) &addrClient, &addrLenClient))){
      perror("accept failed");
      continue;
    }
    
    // send()/recv()
    // TODO
    if(1){
      run_server = 0;
    }



    close(hClient);
  }

  // 
  close(hSocket);

  return EXIT_SUCCESS;
}
