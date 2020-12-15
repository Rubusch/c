// tcp_client01.c
/*
  ipv4 tcp client

  (UNIX Network Programming p.124)
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
#include <signal.h> // sigaction()
#include <sys/time.h> // getitimer(), setitimer() TODO  

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


// signal
void sig_alrm( int signo )
{
        fprintf( stderr, "SIGALRM caught\n" );
        exit( EXIT_SUCCESS );
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



int main( int argc, char* argv[] )
{
        int sockfd;
        struct sockaddr_in servaddr;
        void sig_alrm( int ); // forward declaration of signal handler
        struct itimerval val; // timer object, to be initialized

        // socket
        if (0 > ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ))) {
                perror( "socket failed" );
                exit( EXIT_FAILURE );
        }


        // register signalhandler for SIGALRM
        mysignal( SIGALRM, sig_alrm);

        // set timer to go off and throws SIGALRM before connection finishes
        // itimerval - next value
        val.it_interval.tv_sec = 0;
        val.it_interval.tv_usec = 0;
        // itimerval - current value
        val.it_value.tv_sec = 0;
        val.it_value.tv_usec = 50000; // 50ms
        if (-1 == setitimer( ITIMER_REAL, &val, NULL)) {
                perror( "setitimer error" );
                exit( EXIT_FAILURE );
        }


        // connect - BLOCKING
        memset( &servaddr, 0, sizeof( servaddr ) );
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons( SERV_PORT );
        inet_pton( AF_INET, SERV_ADDR, &servaddr.sin_addr );
        if (0 > connect( sockfd
                         , (struct sockaddr*) &servaddr
                         , sizeof( servaddr )
                    )) {
                perror( "connect failed" );
                exit( EXIT_FAILURE );
        }

        // control
        work_client( sockfd );

        // close optional, exit closes, or func closes
        close( sockfd );
        exit( EXIT_SUCCESS );
}
