// client.c
/*
  simple client
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>  // before <sys/socket.h> !!!
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZ 1024

extern int inet_aton(const char*, struct in_addr*);


int send_text(const int hSocket, const char* message, unsigned int message_len)
{
  if(-1 == send(hSocket, message, message_len, 0)){
    perror("send() failed\n");
    return -1;
  }

  return 0;
}


int receive_text(const int hSocket, char message[BUF_SIZ])
{
  int bytes = 0;

  // maximum size of package is BUF_SIZ!
  bytes = recv(hSocket, message, BUF_SIZ, 0);
  message[bytes] = '\0';

  // doesn't make sense > BUF_SIZ, but anyway
  if((0 >= bytes) || (bytes > BUF_SIZ)){
    perror("terminate connection");
    return -1;
  }

  return 0;
}


int main(int argc, char** argv)
{
  // init varables
  if(argc != 2){
    puts("Server <xxx.xxx.xxx.xxx.>");
    return EXIT_FAILURE;
  }
 
  puts(" *** Client ***\n");
  char* szServerIP = argv[1];
  unsigned int serverPort = 27976;
  printf("set up client for server at:\'%s\' on port \'%i\'\n", szServerIP, serverPort);

  int hSocket = 0;
  struct sockaddr_in server_addr;
  struct hostent *host;
  if(!inet_aton(szServerIP, &server_addr.sin_addr)){
    if(!(host = gethostbyname(szServerIP))){
      perror("gethostbyname() failed");
      return 2;
    }
    server_addr.sin_addr = *(struct in_addr*) host->h_addr;
  }
  server_addr.sin_port = htons(serverPort);
  server_addr.sin_family = AF_INET;


  // socket()
  puts("socket()");
  if(-1 == (hSocket = socket(PF_INET, SOCK_STREAM, 0))){
    perror("socket() faild\n");
    return 1;
  }

  // connect()
  puts("connect()");
  printf("generating socket to %s: %i...\n", inet_ntoa(server_addr.sin_addr), 
	 ntohs(server_addr.sin_port));
  fflush(stdout);
  if(-1 == connect(hSocket, (struct sockaddr*) &server_addr, sizeof(server_addr))){
    perror("connect() failed");
    return 2;
  }
  puts("Ok.");


  // send()/recv()
  puts("send()/recv()");
  char message[BUF_SIZ];
  memset(message, '\0', BUF_SIZ);
  unsigned int message_len = 0;

  unsigned int run = 1;
  while(run){
    // perform action
    if(message_len != 0){
      // send message to server
      printf("send: #%s# - length: %i\n", message, message_len);
      if(0 != send_text(hSocket, message, message_len)){
	perror("send_text() failed\n");
	close(hSocket);
	return EXIT_FAILURE;
      }
      
      // reset
      memset(message, '\0', message_len);

      // receive response
      message_len = BUF_SIZ;
      if(0 != receive_text(hSocket, message)){
	close(hSocket);
	return EXIT_SUCCESS;
      }
      printf("received: #%s# - length: %i\n", message, message_len);
    }

    // init
    memset(message, '\0', BUF_SIZ);
    message_len = strlen(message); // better readable

    // read message input from stdin - max size: BUF_SIZ
    fgets(message, BUF_SIZ, stdin);
    message_len = strlen(message);
    message[message_len-1] = '\0';

    // quit?
    run = strncmp(message, "quit client", message_len);
  };


  // close()  
  puts("close()");
  close(hSocket);

  return EXIT_SUCCESS;
}
