// tcp_server_fork03.c
/*
  tcp server using fork, SIGCHLD and accept handling SIGINTR

  handles SIGINTR arriving from accept()

  do{ ... }while(0); can be replaced here by exit() calls

  tcpserv03.c
  (UNIX Network Programming p.???)
 */

#include <stdlib.h>       // exit()
#include <stdio.h>        // perror()
#include <string.h>       // memset()

#include <sys/types.h>    // bind()
#include <sys/socket.h>   // socket(), bind(), listen()
#include <arpa/inet.h>    // sockaddr_in
#include <unistd.h>       // close()
#include <errno.h>        // errno
#include <limits.h>       // SSIZE_MAX
#include <sys/wait.h>     // waitpid()
#include <signal.h>       // signal(), sigaction()


#define SERV_PORT 12345

// the higher BUFSIZE, the more time it needs to be displayed
#define BUFSIZE 10

// LISTENQ <= tcp_max_syn_backlog of system <= 1024
#define LISTENQ 10

// i/o

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
                fprintf( stderr, "'%s' received - '%d' characters read, '%d' fields in BUF could still be read\n", (char*) vptr, nread, nleft);
                break; // read less than siz characters
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

// control

void work_server( int sockfd )
{
        ssize_t num=0;
        char buf[BUFSIZE]; memset( buf, '\0', BUFSIZE );

        fprintf( stderr, "child connected '%d'\n", sockfd );

        // receive
        if( 0 < (num = readn( sockfd, buf, BUFSIZE) ) ){
                fprintf(stderr, "client says '%s'\n", buf);

                //*
                // var a: answer with "pong"
                strcpy( buf, "pong");
                writen( sockfd, buf, strlen(buf) );
                /*/
                // var b: echo server
                writen( sockfd, buf, num );
                //*/
        }

        // interrupt signal recived
        if( errno == EINTR ){
                perror( "EINTR caught" );
                return;
        }

        // errors
        if( 0 > num ){
                perror( "read error" );
                return;
        }
}

// signal handling

void sig_chld(int signo)
{
        pid_t pid;
        int stat;
        while (0 < (pid = waitpid(-1, &stat, WNOHANG))) {
                fprintf(stderr, "WAITPID - child %d terminated\n", pid);
        }
}

void* mysignal(int signo, void* func)
{
        struct sigaction act, oact;

        act.sa_handler = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        // handle specific cases
        if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
// SunOS 4.x
                act.sa_flags |= SA_INTERRUPT;
#endif
        } else {
#ifdef  SA_RESTART
// SVR4, 44BSD
                act.sa_flags |= SA_RESTART;
#endif
        }

        // sigaction
        if (0 > sigaction(signo, &act, &oact)){
                return(SIG_ERR);
        }
        return oact.sa_handler;
}


int main(int argc, char* argv[])
{
        int listenfd; // listen on connection
        int connfd; // accepted connection
        int res=EXIT_FAILURE;
        pid_t childpid; // for fork
        socklen_t clilen;
        struct sockaddr_in cliaddr, servaddr;
        void sig_chld(int); // signal handler

        // socket
        if( 0 > (listenfd = socket( AF_INET, SOCK_STREAM, 0 ))) {
                perror( "socket failed" );
                exit( EXIT_FAILURE );
        }

        // bind
        memset( &servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl( INADDR_ANY ); // from any device
        servaddr.sin_port = htons( SERV_PORT );
        if ( 0 > bind( listenfd
                       , (struct sockaddr*) &servaddr
                       , sizeof( servaddr )
                     )) {
                perror( "bind failed" );
                exit( EXIT_FAILURE );
        }

        // listen
        if (0 > listen( listenfd, LISTENQ )) {
                perror( "listen failed" );
                exit( EXIT_FAILURE );
        }

        // handle the zombie problem
        if ( SIG_ERR == (mysignal( SIGCHLD, sig_chld ))) {
                perror( "signal error" );
                exit( EXIT_FAILURE );
        }

        do{
                // accept - BLOCKING
                clilen = sizeof(cliaddr);
                if (0 > (connfd = accept( listenfd
                                          , (struct sockaddr*) &cliaddr
                                          , &clilen
                                 ))) {
                        if (errno == EINTR) {
                                continue;
                        }else{
                                perror( "accept failed" );
                                res=EXIT_FAILURE;
                                break;
                        }
                }

                // fork
                if (0 > (childpid = fork())) {
                        perror( "fork failed" );
                        res=EXIT_FAILURE;
                        break;

                }else if (0 == childpid) {
                        // child
                        close( listenfd );
                        work_server( connfd );
                        res=EXIT_SUCCESS;
                        break;
                }
                // parent

                // if child fails to close, close it in the server
                close( connfd );
                res=EXIT_SUCCESS;
        }while(0);
        exit( res );
}
