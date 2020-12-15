// portscanner
/*
  works as client and tries to connect ;-)
  <begin> - begin address, 
  <end>   - end address
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define MIN_PORT 0 // e.g. 0
#define MAX_PORT 100 // e.g. 1024

int main(int argc, char** argv)
{
  struct sockaddr_in addrServer;
  struct servent *servServer=NULL;
  int hSocket=0, port=0;
  unsigned long begin=0, end=0, current=0;

  if((argc < 2) || (argc > 3)){
    fprintf(stderr, "usage: %s <begin> <end>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  begin = ntohl(inet_addr(argv[1])); 
  
  if(argc == 2) end = begin;
  else end = ntohl(inet_addr(argv[2]));
  
  if(begin > end){
    current = end;
    end = begin;
    begin = current;
  }

  for(current = begin; current <= end; ++current){
    addrServer.sin_addr.s_addr = htonl(current);
    printf("%s:\n", inet_ntoa(addrServer.sin_addr));

    for(port = MIN_PORT; port < MAX_PORT; ++port){
      if(-1 == (hSocket = socket(PF_INET, SOCK_STREAM, 0))){
	perror("socket failed");
	return EXIT_FAILURE;
      }

      addrServer.sin_addr.s_addr = htonl(current);
      addrServer.sin_port = htons(port);
      addrServer.sin_family = AF_INET;

      printf("%i\r\n", port);
      fflush(stdout);

      // in case of connect() succeeds: 0, else -1
      if(!connect(hSocket, (struct sockaddr*) &addrServer, sizeof(addrServer))){
	servServer = getservbyport(addrServer.sin_port, "tcp");
	if(servServer) printf("%i (%s) is open\n", port, servServer->s_name);
	else printf("%i (unknown) is open\n", port);
      }
      close(hSocket);
    }
    puts("READY.");
  }
  return EXIT_SUCCESS;
}
