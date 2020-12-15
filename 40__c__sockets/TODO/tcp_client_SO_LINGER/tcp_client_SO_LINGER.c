// tcp_client_SO_LINGER
/*
  ipv4 tcp client
  socket options: SO_LINGER, shutdown tcp socket
  don't wait on timeout

  (UNIX Network Programming p. ???)
 */

#include <stdlib.h> // exit()
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
                return 0; // read less than siz characters
        }
        return siz - nleft;
}



ssize_t writen(int fd, const void *vptr, size_t len)
{
        size_t nleft = len;
        ssize_t nwritten = -1;
        const char *ptr = vptr;
        while (0 < nleft) {
                if (0 >= (nwritten = write(fd, ptr, nleft))) {
                        if (nwritten < 0 && errno == EINTR) {
                                nwritten = 0;
                        } else {
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
        ssize_t num;
        char buf[BUFSIZE]; memset( buf, '\0', BUFSIZE );

        strcpy( buf, "ping" );
        num = strlen(buf) + 1; // will wait untill buffer fills up to BUFSIZE
        //num = BUFSIZE;

        // send
        printf( "send \'%s' with size \'%d\'\n", buf, num );
        writen(sockfd, buf, num);

        // receive
        readn(sockfd, buf, BUFSIZE);
        fprintf(stderr, "server says '%s'\n", buf);
}


int main(int argc, char* argv[])
{
        int sockfd; // connecting fd
        struct linger ling;
        struct sockaddr_in servaddr; // addr obj for server

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
                         , sizeof(servaddr)
                    )) {
                perror( "connect failed" );
                exit( EXIT_FAILURE );
        }

        // control
        work_client( sockfd );

        // set SO_LINGER, don't keep socket alife untill TCP times out
        ling.l_onoff = 1;
        ling.l_linger = 0;
        if( 0 > setsockopt( sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof( ling ) ) ){
                perror("setsockopt failed" );
        }

        // close
        close( sockfd );
        exit( EXIT_SUCCESS );
}
