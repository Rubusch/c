// echoclient.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERV_PORT 12345
#define MAXSIZE 4096


int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in serveraddr;
  char buf[MAXSIZE];
  memset(buf, '\0', MAXSIZE);
  unsigned int echolen;
  int nbytes = 0;
  int read = 0;
  char server_ip[16];
  memset(server_ip, '\0', 16);
  char server_port[6];
  memset(server_port, '\0', 6);
  char text[4096];
  memset(text, '\0', 4096);

//  strcpy(server_ip, "192.168.60.7");
  strcpy(server_ip, "127.0.0.1");
  strcpy(server_port, "12345");

  strcpy(text,
         "<?xml version=\"1.0\"?> \n<TASK task_name=\"guiagent\">\n  <DATA_OUT "
         "name=\"xxx\" dim=\"0\" type=\"cmd\" value=\"agtstart\">\n <DST "
         "name=\"agentname\" />\n </DATA_OUT>\n </TASK>");

  puts("socket()");
  if (0 > (sockfd = socket(PF_INET, SOCK_STREAM,
                           IPPROTO_TCP))) { // PF_INET for packets
    perror("socket() failed");
    exit(EXIT_FAILURE);
  }

  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET; // AF_INET for addresses
  serveraddr.sin_addr.s_addr = inet_addr(server_ip);
  serveraddr.sin_port = htons(atoi(server_port));

  puts("connect()");
  if (0 >
      connect(sockfd, ( struct sockaddr * )&serveraddr, sizeof(serveraddr))) {
    perror("connect() failed");
    exit(EXIT_FAILURE);
  }
  puts("connect() - ok");

  puts("send()");
  echolen = strlen(text);
  if (echolen != send(sockfd, text, echolen, 0)) {
    perror("send() failed");
    exit(EXIT_FAILURE);
  }
  puts("send() - ok");

  fprintf(stdout, "received: ");
  while (nbytes < echolen) {
    read = 0;
    puts("recv()");
    if (1 > (read = recv(sockfd, buf, MAXSIZE - 1, 0))) {
      perror("recv() failed");
      exit(EXIT_FAILURE);
    }
    puts("recv() - ok");
    nbytes += read;
    buf[read] = '\0';
    puts(buf);
  }
  close(sockfd);

  exit(EXIT_SUCCESS);
}
