//work_server.c

#include <stdio.h> // perror()
#include <string.h> // memset()
#include <errno.h> // errno

#define BUFSIZE 1024

size_t readn( int fd, void *vptr, size_t siz );
size_t writen( int fd, const void *vptr, size_t len );

int work_server( int sockfd )
{
        size_t num=0;
        char buf[BUFSIZE]; memset( buf, '\0', BUFSIZE );

        fprintf( stderr, "\t%s:%d - child connected '%d'\n", __FILE__, __LINE__ , sockfd );

        // receive
        if (0 < (num = readn( sockfd, buf, BUFSIZE ))) {
                fprintf( stderr, "client says '%s'\n", buf );

                // send back
                //*
                // var a: answer with "pong"
                strcpy( buf, "pong");
                writen( sockfd, buf, strlen(buf) );
                /*/
                // var b: echo server
                writen( sockfd, buf, num );
                //*/
        }

        // interruption
        if (errno == EINTR) {
                perror( "EINTR - interrupted function call" );
                return -1;
        }

        // errors
        if (0 > num) {
                perror( "read error" );
                return -1;
        }
        return 0;
}
