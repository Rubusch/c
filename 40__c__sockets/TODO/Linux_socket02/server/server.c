// server.c
/*
  simple echo server
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h> // before <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZ 1024


/*
  handle the messages received from the client and send a response (the same message)
//*/
int handle_client( const int hSocket)
{
  char buffer[BUF_SIZ];
  memset(buffer, '\0', BUF_SIZ);
  unsigned int bytes = 0;

  printf("5. handle client\n");
  unsigned int run_client = 1;
  while((bytes = recv(hSocket, buffer, sizeof(buffer), 0)) > 0){
    // receive
    printf("received: #%s# - bufferlen: %i\n", buffer, sizeof(buffer));

    // perform action
    if((11 == strlen(buffer)) && (0 == strncmp("quit server", buffer, 11))){ 
      memset(buffer, '\0', BUF_SIZ);
      strncpy(buffer, "quit client", strlen("quit client"));
      return 1;
    }
    printf("send #%s#\n", buffer);
    send(hSocket, buffer, bytes, 0);    
  }

  //  if((11 == strlen(buffer)) && (0 == strncmp("quit server", buffer, 11))) 
  //    return 1; // XXX
  return 0;
}


void quit(int hSocket, int hClient){
  close(hClient);
  close(hSocket);
  exit(0);
}


int main(int argc, char** argv)
{
  printf(" *** Server ***\n\n");

  // 0. init
  printf("0. init server\n");
  int hSocket = 0, hClient = 0;
  

  // 1. socket()
  printf("1. socket()\n");
  if(-1 == (hSocket = socket(PF_INET, SOCK_STREAM, 0))){
    perror("socket() failed"); 
    return 1;
  }


  // 2. bind()
  printf("2. bind()\n");
  struct sockaddr_in client;
  client.sin_addr.s_addr = INADDR_ANY;
  client.sin_port = htons(27976);
  client.sin_family = AF_INET;
  if( -1 == bind(hSocket, (struct sockaddr*) &client, sizeof(client))){
    perror("bind() failed");
    return 2;
  }


  // 3. listen() - only server
  printf("3. listen()\n");
  if(-1 == listen(hSocket, 3)){
    perror("listen() failed");
    return 3;
  }


  // 4. accept() - only server
  unsigned int run_server = 1;
  while(run_server){

    printf("4. accept()\n");
    socklen_t client_len = sizeof(hSocket);
    if(-1 == (hClient = accept(hSocket, (struct sockaddr*) &client, &client_len))){
      perror("accept() failed");
      continue;
    }

    printf("Client from %s\n", inet_ntoa(client.sin_addr));
    if(1 == handle_client(hClient)) quit(hSocket, hClient);

    printf("6. close(hClient)\n");
    close(hClient);
  }

  // 7. close()
  printf("7. close(hServer)\n");
  close(hSocket);
  return EXIT_SUCCESS;
}

