// tcp_client02.c
/*
  ipv4 tcp client

  multiple sockets, SOCKETQ

  (UNIX Network Programming p.???)
//*/

#include <stdlib.h>
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/socket.h> // socket(), inet_pton()
#include <sys/types.h> // inet_pton()
#include <arpa/inet.h> // sockaddr_in, inet_pton()
#include <errno.h> // errno
#include <unistd.h> // read(), write()
#include <limits.h> // SSIZE_MAX

#define SERV_PORT 12345
#define SERV_ADDR "127.0.0.1"
#define BUFSIZE 1024

#define SOCKETQ 1024


ssize_t readn( int fd, void *vptr, size_t siz )
{
        size_t nleft = --siz; // we omit the '\0'
        ssize_t nread = -1;
        char *ptr = vptr;

        while( 0 < nleft ){
                // looping over read(fd,...) and just picking out 'nleft'
                // characters results in first picking out 'nleft' characters
                // and in the next turn picking out 0 characters, thus break,
                // thus only the first 'nleft' characters will be read here, at
                // most SSIZE_MAX (e.g. something like 2147483647)
                //
                // nleft must be lesser than SSIZE_MAX for read
                if( 0 > ( nread = read( fd, ptr, nleft%SSIZE_MAX ))){
                        if( errno == EINTR ){
                                nread = 0;
                        }else{
                                return -1;
                        }

                }else if( 0 == nread ){
                        fprintf( stderr, "read nothing\n" );
                        break;
                }
                nleft -= nread;
                ptr += nread;
//                fprintf( stderr, "'%s' received - '%d' characters read, '%d' fields in BUF could still be read\n", (char*) vptr, nread, nleft);
                return 0; // read less than siz characters
        }
        return (siz - nleft);
}


ssize_t writen( int fd, const void *vptr, size_t len )
{
        size_t nleft = len;
        ssize_t nwritten = -1;
        const char *ptr = vptr;

        while( 0 < nleft ){
                if (0 >= (nwritten = write( fd, ptr, nleft))) {
                        if( nwritten < 0 && errno == EINTR){
                                nwritten = 0;
                        }else{
                                return -1;
                        }
                }
                nleft -= nwritten;
                ptr += nwritten;
        }
        return len;
}


void work_client( int sockfd )
{
        ssize_t num=0;
        char buf[BUFSIZE];

        memset( buf, '\0', BUFSIZE );
        strcpy( buf, "ping" );
        num = strlen(buf) + 1; // will wait untill buffer fills up to BUFSIZE
        //num = BUFSIZE;

        // send
        printf( "send \'%s' with size \'%d\'\n", buf, num );
        writen(sockfd, buf, num);

        sleep( 1 );

        // receive
        readn(sockfd, buf, BUFSIZE);
        printf("server says '%s'\n", buf);
}


int main( int argc, char* argv[] )
{
        int sockfd[SOCKETQ]; memset( sockfd, 0, SOCKETQ*sizeof(*sockfd));
        int idx=-1;
        struct sockaddr_in servaddr;

        for (idx=0; idx<SOCKETQ; ++idx) {
                fprintf(stderr, "open sockfd[%d]\n", idx);

                // socket
                if (0 > ( sockfd[idx] = socket( AF_INET, SOCK_STREAM, 0 ))) {
                        perror( "socket failed" );
                        exit( EXIT_FAILURE );
                }

                // connect - BLOCKING
                memset( &servaddr, 0, sizeof( servaddr ) );
                servaddr.sin_family = AF_INET;
                servaddr.sin_port = htons( SERV_PORT );
                inet_pton( AF_INET, SERV_ADDR, &servaddr.sin_addr );
                if (0 > connect( sockfd[idx]
                                 , (struct sockaddr*) &servaddr
                                 , sizeof(servaddr)
                            )) {
                        perror( "connect failed" );
                        exit( EXIT_FAILURE );
                }
        }

        // control only on 1. socket
        work_client( sockfd[0] );

        // close optional, exit closes, or func closes
        for (idx=0; idx<SOCKETQ; ++idx) {
                close( sockfd[idx] );
        }
        exit( EXIT_SUCCESS );
}
