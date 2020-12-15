// comm_c.c - Socket Client (Linux)
/*
  compile:
  gcc -o comm_s comm_s.c
  
  Each end system needs one server (comm_s <port>) and can have one to many 
  clients (comm_c <target system> <port>).

  The input at the client will be sent to the server.
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


#define BUFSIZE 1024
char buf[BUFSIZE + 1];

extern in_addr_t inet_addr(const char* cp);
extern size_t write( int sockfd, void* data, size_t data_len);
extern int close(int sockfd);


/*
  main program (client):
  - process arguments
  - open socket and establish connection to the server
  - read text linewise and send it via the connection
  - in case of CTRL + D close connection
*/
int main(int argc, char** argv)
{
  int s;
  struct sockaddr_in addr;
  //  char *p;
  if(argc != 3){
    fprintf(stderr, "Usage: %s <address> <port>\n", argv[0]);
    exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argv[2]));
  addr.sin_addr.s_addr = inet_addr(argv[1]);

  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket");
    exit(1);
  }

  if(connect(s, (struct sockaddr*) &addr, sizeof(addr))){
    perror("connect"); // refused?!
    exit(1);
  }

  buf[BUFSIZE] = 0;
  while(fgets(buf, BUFSIZE, stdin) != NULL){
    if(write(s, buf, strlen(buf)) == 0){
      perror("write");
      break;
    }
  }

  close(s);
  exit(0);
}

  
