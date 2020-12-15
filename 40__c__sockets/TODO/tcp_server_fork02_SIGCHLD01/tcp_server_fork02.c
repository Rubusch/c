// tcp_server_fork02.c
/*
  tcp server using fork with SIGCHLD

  handles the zombie problem using SIGCHLD and a signal implementation, using
  sigaction

  the do{...}while(0); is just a variation, and can be replaced by exit calls

  tcpserv02.c
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

/*
  3. step: signal handler
  the function will be called when a child terminates, and a SIGCHLD is received
 */
void sig_chld(int signo)
{
        pid_t pid;
        int stat;
/*
        // wait() implementation
        //
        // The wait() system call suspends execution of the calling process
        // until one of its children terminates. The call wait(&status) is
        // equivalent to: waitpid(-1, &status, 0);
        pid = wait(&stat);
        fprintf(stderr, "WAIT - child %d terminated\n", pid);
/*/
        // waitpid() implementation (more powerfull)
        //
        // All of these system calls are used to wait for state changes in a
        // child of the calling process, and obtain information about the child
        // whose state has changed. A state change is considered to be: the
        // child terminated; the child was stopped by a signal; or the child was
        // resumed by a signal.
        //
        // -1 - meaning wait for any child process.
        // WNOHANG - return immediately if no child has exited.
        while (0 < (pid = waitpid(-1, &stat, WNOHANG))) {
                fprintf(stderr, "WAITPID - child %d terminated\n", pid);
        }
//*/
}

/*
  2. step: signal impelmentation
  connect signal to signalhandler, here sig_chld()
 */
void* mysignal(int signo, void* func)
{
        struct sigaction act, oact;

        // connect signal to func
        act.sa_handler = func;

        // initialize the signal set given by set to empty, with all signals
        // excluded from the set.
        sigemptyset(&act.sa_mask);

        // reset flag
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
                // examine and change a signal action
                return(SIG_ERR);
        }
        return oact.sa_handler;
}


int main( int argc, char** argv)
{
        int listenfd; // listen on connection
        int connfd; // accepted connection
        int res=EXIT_FAILURE;
        pid_t childpid; // for fork
        socklen_t clilen;
        struct sockaddr_in cliaddr, servaddr;
        // declaration for the signal mech; it is called when child dies
        void sig_chld(int);

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

        // The zombie problem
        //
        // if the parent runs for a long time and accepts many connections, each
        // of these connections will create a zombie when the connection is
        // terminated. A zombie is a process which has terminated but but cannot
        // be permitted to fully die because at some point in the future, the
        // parent of the process might execute a wait and would want information
        // about the death of the child. Zombies clog up the process table in
        // the kernel, and so they should be prevented. Unfortunately, the code
        // which prevents zombies is not consistent across different
        // architectures. When a child dies, it sends a SIGCHLD signal to its
        // parent.
        // (http://www.linuxhowtos.org/C_C++/socket.htm)
        //

        // 1. step: signal()
/*
        // signal() sets the disposition of the signal signum to handler
        //
        // BUT:
        // The behavior of signal() varies across UNIX versions, and has also
        // varied historically across different versions of Linux. Avoid its
        // use: use sigaction(2) instead.
        // (man 2 signal)

        // use library signal function
        if ( SIG_ERR == (signal( SIGCHLD, sig_chld ))) {
/*/
        // THEREFORE:
        // customized signal disposition function
        if ( SIG_ERR == (mysignal( SIGCHLD, sig_chld ))) {
//*/
                perror( "signal error" );
                exit( EXIT_FAILURE );
        }

        do{
                // accept - BLOCKING
                clilen = sizeof(cliaddr);
                if (0 > (connfd = accept( listenfd
                                         , (struct sockaddr*) &cliaddr
                                         , &clilen ))) {
                        perror( "accept failed" );
                        res=EXIT_FAILURE;
                        break;
                }

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
