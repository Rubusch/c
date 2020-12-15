// tcp_server_select01.c
/*
  ipv4 tcp server
  multiplexing: select

  maximum socket queue to listen on can be seen via:
  $ sudo sysctl net.ipv4.tcp_max_syn_backlog
  net.ipv4.tcp_max_syn_backlog = 512
  should not be more than 1024

  (UNIX Network Programming, p. 179)
//*/

#include <stdlib.h> // exit()
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/types.h> // bind()
#include <sys/socket.h> // socket(), bind(), listen()
#include <arpa/inet.h> // sockaddr_in
#include <unistd.h> // close()
#include <errno.h> // errno


#define SERV_PORT 12345
#define SERV_ADDR "127.0.0.1"
#define BUFSIZE 10
// LISTENQ <= tcp_max_syn_backlog of system <= 1024
#define LISTENQ 256


ssize_t readn( int fd, void *vptr, size_t num )
{
        size_t nleft = num;
        ssize_t nread = -1;
        char *ptr = vptr;

        while( 0 < nleft ){
                if( 0 > ( nread = read( fd, ptr, nleft))){
                        if( errno == EINTR ){
                                nread = 0;
                        }else{
                                return -1;
                        }
                }else if( 0 == nread ){
                        break;
                }
                nleft -= nread;
                ptr += nread;
        }
        return num - nleft;
}


ssize_t writen( int fd, const void *vptr, size_t num )
{
        size_t nleft = num;
        ssize_t nwritten = -1;
        const char *ptr = vptr;

        while( 0 < nleft ){
                if( 0 >= (nwritten = write( fd, ptr, nleft))) {
                        if( nwritten < 0 && errno == EINTR){
                                nwritten = 0;
                        }else{
                                return -1;
                        }
                }
                nleft -= nwritten;
                ptr += nwritten;
        }
        return num;
}


int main( int argc, char* argv[] )
{
        int idx, maxidx;
        int maxfd;
        int listenfd; // listen on new fd's
        int connfd; // accepted listen fd
        int sockfd; // pointer in loop to current fd
        int nready, client[FD_SETSIZE];
        ssize_t num;
        fd_set rset, allset; // select - rset, actually checked; allset, for registering fd's
        char buf[BUFSIZE]; memset( buf, '\0', BUFSIZE );
        socklen_t clilen;
        struct sockaddr_in cliaddr, servaddr;

        // socket
        if ( 0 > (listenfd = socket( AF_INET, SOCK_STREAM, 0 ))) {
                perror( "socket failed" );
                exit( EXIT_FAILURE );
        }

        // bind
        memset( &servaddr, 0, sizeof( servaddr ));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
        servaddr.sin_port = htons( SERV_PORT );
        if ( 0 > bind( listenfd
                       , (struct sockaddr*) &servaddr
                       , sizeof( servaddr ))) {
                perror( "bind failed" );
                exit( EXIT_FAILURE );
        }

        // listen
        if ( 0 > listen( listenfd, LISTENQ )) {
                perror( "listen failed" );
                exit( EXIT_FAILURE );
        }

        // client pool - init values and list entries to -1
        maxfd = listenfd; // initialize
        maxidx = -1; // index into client[] array
        for( idx=0; idx < FD_SETSIZE; ++idx ){
                client[idx] = -1; // -1 indicates available entry
        }

        // select - zero allset
        fprintf( stderr, "FD_ZERO( &allset )\n" );
        FD_ZERO( &allset );

        // select - register listenfd in allset
        fprintf( stderr, "FD_SET( listenfd, &allset )\n" );
        FD_SET( listenfd, &allset );


        while( 1 ){
                // select - structure assignment allset to rset!!!
                rset = allset;

                // select - call
                fprintf( stderr, "SELECT on &rset\n" );
                if (0 > (nready = select( maxfd + 1
                                          , &rset
                                          , NULL
                                          , NULL
                                          , NULL ))) {
                        /*
                          select return value

                          On success, select() and pselect() return the number
                          of file descriptors contained in the three returned
                          descriptor sets (that is, the total number of bits
                          that are set in readfds, writefds, exceptfds) which
                          may be zero if the timeout expires before anything
                          interesting happens. On error, -1 is returned, and
                          errno is set appropriately; the sets and timeout
                          become undefined, so do not rely on their contents
                          after an error.
                        */
                        perror( "select failed" );
                        exit( EXIT_FAILURE );
                }


                // select - listenfd available for read and write
                if ( FD_ISSET( listenfd, &rset )) { // new client connection
                        fprintf( stderr, "FD_ISSET( listenfd, &rset )\n");

                        // accept
                        clilen = sizeof( cliaddr );
                        if ( 0 > (connfd = accept( listenfd
                                                   , (struct sockaddr*) &cliaddr
                                                   , &clilen ))) {
                                perror( "accept failed" );
                                exit( EXIT_FAILURE );
                        }

                        // client pool - register new client
                        for (idx=0; idx < FD_SETSIZE; ++idx) {
                                if (0 > client[idx] ) {
                                        fprintf( stderr, "\tregister new client\n" );
                                        client[idx] = connfd; // save descriptor
                                        break;
                                }
                                fprintf( stderr, "\tdrop new client\n" );
                        }

                        // client pool - check if full
                        if (idx == FD_SETSIZE){
                                perror("too many clients");
                                exit(EXIT_FAILURE);
                        }

                        // select - register connfd in allset
                        FD_SET(connfd, &allset);
                        if (connfd > maxfd) {
                                fprintf( stderr, "\tupdate maxfd for select\n");
                                maxfd = connfd;
                        }

                        // max index in client[] array
                        if (idx > maxidx) {
                                fprintf(stderr, "\tupdate maxidx to new number of clients \n" );
                                maxidx = idx;
                        }

                        // no more readable descriptors
                        if (--nready <= 0) {
                                fprintf( stderr, "\tselect return value '%d' - registered all, no more readable descriptors left\n", nready );
                                continue;
                        }
                }


                fprintf( stderr, "loop over all clients to check for data\n");
                for (idx=0; idx <= maxidx; ++idx) { // loop over all clients to check for data
                        if (0 > (sockfd = client[idx])) {
                                continue; // -1, no client fd set
                        }


                        // select - sockfd available for read and write
                        if (FD_ISSET(sockfd, &rset)) {
                                fprintf( stderr, "FD_ISSET( sockfd, &rset )\n" );
                                if (0 > (num = read( sockfd, buf, BUFSIZE ))) {
                                        perror( "read failed" );
                                        exit( EXIT_FAILURE );

                                }else if (0 == num) {
                                        fprintf( stderr, "\tconnection closed by client\n");

                                        if ( 0 > close( sockfd ) ){
                                                perror( "close failed" );
                                                exit( EXIT_FAILURE );
                                        }

                                        // select - clear sockfd from allset
                                        fprintf( stderr, "FD_CLR( sockfd, &allset )\n" );
                                        FD_CLR( sockfd, &allset );
                                        client[idx] = -1;
                                }else{
                                        // work
                                        fprintf(stderr, "\tclient says '%s', answering...\n", buf);

                                        //*
                                        // var a: answer "ping"
                                        memset(buf, '\0', BUFSIZE);
                                        strcpy(buf, "pong");
                                        writen( sockfd, buf, num );
                                        /*/
                                        // var b: send echo
                                        writen( sockfd, buf, num );
                                        //*/
                                }

                                if (--nready <= 0) {
                                        fprintf(stderr, "\tselect return value '%d' - nothing left to process\n", nready);
                                        break; // no more readable descriptors
                                }
                        }


                }
        }
}
