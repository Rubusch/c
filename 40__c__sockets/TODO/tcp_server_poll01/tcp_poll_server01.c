// tcp_poll_server01.c
/*
  tcpserver, poll() implementation

  poll() performs a similar task to select(2): it waits for one of a set of file
  descriptors to become ready to perform I/O.

  tcpcliserv/tcpservpoll01.c
  (UNIX Network Programming, p.187)
//*/


/* library info

  OPEN_MAX in /usr/include/bits/stdio_lim.h

  "We do not provide fixed values for
  (...)
  OPEN_MAX      Maximum number of files that one process can have open
                at anyone time."
*/

/* library info

  definition of POLLRDNORM
  either generally
#define _GNU_SOURCE 1

  or more specific
#define _XOPEN_SOURCE 1

  alternatively use
  POLLIN
*/
#define OPEN_MAX 256


#include <stdlib.h>                // exit()
#include <stdio.h>                 // perror()
#include <string.h>                // memset()

#include <sys/types.h>             // bind()
#include <sys/socket.h>            // socket(), bind(), listen()
#include <arpa/inet.h>             // sockaddr_in
#include <unistd.h>                // close()
#include <errno.h>                 // errno
#include <sys/poll.h>              // struct pollfd, poll()
#include <sys/un.h>                // sockaddr_un

#define SERV_PORT 12345
#define MAXLINE 10
#define LISTENQ 10

// Specifying a negative value in timeout means an infinite timeout (man 2 poll)
#define INFTIM -1


/*
  display connection data for debugging
//*/
char* sock_ntop( const struct sockaddr* sa, socklen_t salen )
{
        char portstr[8]; memset( portstr, '\0', 8 );
        static char str[128]; memset( str, '\0', 128 ); // Unix domain is largest

        do{
                switch (sa->sa_family) {
                case AF_INET:
                {
                        struct sockaddr_in* sin = (struct sockaddr_in*) sa;

                        // convert IPv4 and IPv6 addresses from binary to text form
                        if (NULL == inet_ntop( AF_INET
                                               , &sin->sin_addr
                                               , str
                                               , sizeof(str)
                                    ) ) {
                                break;
                        }

                        // get port number, as string
                        if (0 != ntohs(sin->sin_port) ) {
                                snprintf( portstr
                                          , sizeof(portstr)
                                          , ":%d"
                                          , ntohs(sin->sin_port)
                                        );
                                strcat(str, portstr);
                        }

                        return str;
                }

#ifdef IPV6
                case AF_INET6:
                {
                        struct sockaddr_in6 *sin6 = (struct sockaddr_in6*) sa;

                        str[0] = '[';
                        if (NULL == inet_ntop( AF_INET6
                                               , &sin6->sin6_addr
                                               , str + 1
                                               , sizeof(str) - 1
                                    ) ) {
                                break;
                        }

                        if (0 != ntohs(sin6->sin6_port) ) {
                                snprintf( portstr
                                          , sizeof(portstr)
                                          , "]:%d"
                                          , ntohs(sin6->sin6_port)
                                        );
                                strcat(str, portstr);
                                return str;
                        }

                        return (str + 1);
                }
#endif

#ifdef AF_UNIX
                case AF_UNIX:
                {
                        struct sockaddr_un* unp = (struct sockaddr_un*) sa;

                        // OK to have no pathname bound to the socket: happens on
                        // every connect() unless client calls bind() first.
                        if (0 == unp->sun_path[0]) {
                                strcpy(str, "(no pathname bound)");
                        }else{
                                snprintf(str, sizeof(str), "%s", unp->sun_path);
                        }

                        return str;
                }
#endif

#ifdef HAVE_SOCKADDR_DL_STRUCT
// Define to 1 if <net/if_dl.h> defines struct sockaddr_dl
                case AF_LINK:
                {
                        struct sockaddr_dl *sdl = (struct sockaddr_dl*) sa;

                        if (0 < sdl->sdl_nlen){
                                snprintf( str
                                          , sizeof(str)
                                          , "%*s (index %d)"
                                          , sdl->sdl_nlen
                                          , &sdl->sdl_data[0]
                                          , sdl->sdl_index
                                        );
                        } else {
                                snprintf( str
                                          , sizeof(str)
                                          , "AF_LINK, index=%d"
                                          , sdl->sdl_index
                                        );
                        }

                        return str;
                }
#endif

                default:
                        if (0 > snprintf( str
                                          , sizeof(str)
                                          , "sock_ntop: unknown AF_xxx: %d, len %d"
                                          , sa->sa_family
                                          , salen
                                    ) ){
                                break;
                        }
                        return str;
                }
        }while( 0 );

        perror( "sock_ntop failed" );
        return NULL;
}


ssize_t writen( int fd, const void *vptr, size_t len )
{
        size_t nleft = len;
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
        return len;
}


int main( int argc, char** argv)
{
        int idx=-1, maxidx=-1;
        ssize_t num;
        int nready;                     // ready fd's
        int listenfd=-1;                // fd for waiting on connections
        int connfd=-1;                  // accepted connection fd
        int sockfd=-1;                  // fd for looping through fd list
        char buf[MAXLINE]; memset( buf, '\0', MAXLINE );
        socklen_t clilen;
        struct pollfd client[OPEN_MAX]; // array of client fd's
        struct sockaddr_in cliaddr;     // address object for accepting socket connections
        struct sockaddr_in servaddr;    // address object for binding to this server

        // socket
        if( 0 > (listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) ){
                perror( "socket failed" );
                exit(EXIT_FAILURE);
        }

        // bind
        memset( &servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl( INADDR_ANY ); // in case, bind to all interfaces
        servaddr.sin_port = htons( SERV_PORT );
        if( 0 > bind( listenfd
                      , (struct sockaddr*) &servaddr
                      , sizeof( servaddr ) ) ) {
                perror( "bind failed" );
                exit( EXIT_FAILURE );
        }

        // listen
        if( 0 > listen( listenfd, LISTENQ ) ){
                perror( "listen failed" );
                exit( EXIT_FAILURE );
        }

        // poll - init struct pollfd array: client[]
        client[0].fd = listenfd;
#ifdef _XOPEN_SOURCE
        fprintf( stderr, "_XOPEN_SOURCE was defined\n");
        // Equivalent to POLLIN (_XOPEN_SOURCE). There is data to read.
        client[0].events = POLLRDNORM;
#else
        fprintf( stderr, "_XOPEN_SOURCE was NOT defined\n");
        // There is data to read.
        client[0].events = POLLIN;
#endif
        for (idx = 1; idx < OPEN_MAX; ++idx){
                // -1 indicates available entry
                client[idx].fd = -1;
        }
        maxidx = 0; // max index into client[] array

        while ( 1 ) {
                // poll - waits for one of a set of file descriptors (client[])
                // to become ready to perform I/O.
                if( 0 >= (nready = poll( client, maxidx+1, INFTIM ))){
                        /*
                          On success, a positive number is returned; this is the
                          number of structures which have nonzero revents fields
                          (in other words, those descriptors with events or
                          errors reported). A value of 0 indicates that the call
                          timed out and no file descriptors were ready. On error,
                          -1 is returned, and errno is set appropriately.
                        */
                        perror( "poll failed" );
                        exit( EXIT_FAILURE );
                }

#ifdef _XOPEN_SOURCE
                if (client[0].revents & POLLRDNORM) { // new client connection, _XOPEN_SOURCE
#else
                if (client[0].revents & POLLIN) { // new client connection
#endif
                        // accept to cliaddr
                        clilen = sizeof( cliaddr );
                        if( 0 > (connfd = accept( listenfd
                                                  , (struct sockaddr*) &cliaddr
                                                  , &clilen ) ) ){
                                perror( "accept failed" );
                                exit( EXIT_FAILURE );
                        }

                        fprintf( stderr, "new client: %s\n", sock_ntop((struct sockaddr*) &cliaddr, clilen));

                        // get latest free place in fd list, to store new fd
                        for (idx=1; idx<OPEN_MAX; ++idx) {
                                if (0 > client[idx].fd) {
                                        client[idx].fd = connfd; // save descriptor
                                        break;
                                }
                        }

                        if (idx == OPEN_MAX) {
                                perror( "too many clients" );
                        }

#ifdef _XOPEN_SOURCE
                        // there is data to read
                        client[idx].events = POLLRDNORM;
#else
                        client[idx].events = POLLIN;
#endif
                        if (maxidx < idx) {
                                maxidx = idx; // max index in client[] array
                        }

                        if (0 >= --nready) {
                                continue; // no more readable descriptors
                        }
                }


                // check all clients for data
                for (idx = 1; idx <= maxidx; ++idx) {

                        // get client (current loop pointer: sockfd)
                        if (0 > (sockfd = client[idx].fd) ) {
                                continue;
                        }
#ifdef _XOPEN_SOURCE
                        if (client[idx].revents & (POLLRDNORM | POLLERR) ) {
#else
                        if (client[idx].revents & (POLLIN | POLLERR)) {
#endif

                                // read from socket...
                                if (0 > (num = read(sockfd, buf, MAXLINE) ) ) {
                                        // ...error
                                        if (errno == ECONNRESET) {
                                                // connection reset by client
                                                fprintf(stderr, "client[%d] aborted connection\n", idx);
                                                close(sockfd);
                                                client[idx].fd = -1;
                                        } else {
                                                perror("read error");
                                        }

                                } else if (num == 0) {
                                        // ...nothing - connection closed by client
                                        fprintf(stderr, "client[%d] closed connection\n", idx);
                                        close(sockfd);
                                        client[idx].fd = -1;

                                } else {
                                        // ...something
                                        fprintf( stderr, "client[%d] says: '%s'\n", idx, buf );
                                        //*
                                        // var a: answer "Pong"
                                        strcpy( buf, "pong" );
                                        writen( sockfd, buf, strlen( buf ) );
                                        /*/
                                        // var b: echo server
                                        writen( sockfd, buf, num );
                                        //*/
                                }

                                // check if no more readable descriptors
                                if (--nready <= 0){
                                        break;
                                }
                        }
                }
        }

        exit(EXIT_SUCCESS);
}
