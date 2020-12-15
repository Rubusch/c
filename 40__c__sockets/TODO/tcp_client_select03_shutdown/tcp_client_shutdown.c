// tcp_client_select.c
/*
  ipv4 tcp client
  shutdown

  test shutdown(fd,SHUT_RD) on server and see what happens

  (UNIX Network Programming p. ???)
//*/

#include <stdlib.h> // exit()
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/socket.h> // socket(), inet_pton()
#include <sys/types.h> // inet_pton()
#include <arpa/inet.h> // sockaddr_in, inet_pton()
#include <unistd.h> // sleep(), close()

#define SERV_PORT 12345
#define SERV_ADDR "127.0.0.1"

int main(int argc, char* argv[])
{
        int sockfd;
        struct sockaddr_in servaddr;

        // socket
        if (0 > (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
                perror("socket failed");
                exit(EXIT_FAILURE);
        }

        // connect
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);
        if (0 > connect( sockfd
                         , (struct sockaddr*) &servaddr
                         , sizeof( servaddr )
                    )) {
                perror( "connect failed" );
                exit( EXIT_FAILURE );
        }

        // control
        shutdown( sockfd, SHUT_RD );
        sleep( 10 );

        // close optional, exit closes, or func closes
        close( sockfd );
        exit( EXIT_SUCCESS );
}
