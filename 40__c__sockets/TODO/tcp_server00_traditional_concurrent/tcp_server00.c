// tcp_server00.c
/*
  resources:
  serv00.c; UNIX Network Programming, p. XXX


  traditional concurrent server: use as base level

  <valgrind checked>
*/

#include <stdlib.h> // exit(), atoi()
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/types.h> // bind()
#include <sys/socket.h> // socket(), bind(), listen()
#include <arpa/inet.h> // sockaddr
#include <unistd.h> // close()
#include <errno.h> // errno
#include <signal.h> // SIG_ERR
#include <sys/resource.h> // getrusage()

/// struct addrinfo
#define _XOPEN_SOURCE 1
#include <netdb.h> // struct addrinfo, getaddrinfo()

// LISTENQ <= tcp_max_syn_backlog of system <= 1024
#define LISTENQ 10

// debug
#define MALLOC_CHECK_ 1


void sig_int( int signo )
{

        double user, sys;
        struct rusage myusage, childusage;
        fprintf( stderr, "\t%s:%d - SIGINT caught\n", __FILE__, __LINE__ );

        // comment out, in case not available
        int getrusage( int, struct rusage* );
        if (0 > getrusage( RUSAGE_SELF, &myusage )) {
                perror( "getrusage failed" );
        }

        if( 0 > getrusage( RUSAGE_CHILDREN, &childusage)) {
                perror( "getrusage failed" );
        }

        user = (double) myusage.ru_utime.tv_sec \
                + myusage.ru_utime.tv_usec / 1000000.0;

        user += (double) childusage.ru_utime.tv_sec \
                + childusage.ru_utime.tv_usec / 1000000.0;

        sys = (double) myusage.ru_stime.tv_sec \
                + myusage.ru_stime.tv_usec / 1000000.0;

        sys += (double) childusage.ru_stime.tv_sec \
                + childusage.ru_stime.tv_usec / 1000000.0;

        fprintf( stderr, "\nuser time = '%g', sys time = '%g'\n", user, sys);

        exit(EXIT_SUCCESS);
}


int main( int argc, char* argv[] )
{
        int listenfd = -1; // socket to listen on
        int connfd = -1; // socket for captured connection
        socklen_t clilen = -1; // length of client address
        struct sockaddr *cliaddr = NULL; // client address
        char serv_port[16]; memset( serv_port, '\0', 16 ); // service port
        char dev_ip[16]; memset( dev_ip, '\0', 16 ); // optionally specified, device ip
        char *host = NULL;
        struct addrinfo addrnfo_hints; // init object for address info
        struct addrinfo *addrnfo_res = NULL; // result structure for getaddrinfo()
        struct addrinfo *addrnfo_res_save = NULL; // save removal pointer
        const int on=1; // socket options
        int num=-1;
        int res=EXIT_FAILURE;

        // serv_port, dev_ip
        if (argc == 2) {
                fprintf( stderr, "listen on port: '%s'\n", argv[1]);
                strncpy( serv_port, argv[1], 16 );
                serv_port[15]='\0';
        } else if (argc == 3) {
                strncpy( dev_ip, argv[1], 16 );
                dev_ip[15] = '\0';
                host = dev_ip;
                strncpy( serv_port, argv[2], 16 );
                serv_port[15]='\0';
        } else {
                fprintf( stderr, "usage: %s <port>\n", argv[0] );
                exit( res );
        }

        // getaddrinfo
        /* should return socket addresses for any address family (either IPv4 or
           IPv6, for example) that can be used
        */
        memset( &addrnfo_hints, 0, sizeof( struct addrinfo ) );
        addrnfo_hints.ai_flags = AI_PASSIVE;
        addrnfo_hints.ai_family = AF_UNSPEC;
        addrnfo_hints.ai_socktype = SOCK_STREAM;

        // any protocol, to avoid issue: ai_socktype not supported
        addrnfo_hints.ai_protocol = 0;

        addrnfo_hints.ai_canonname = NULL;
        addrnfo_hints.ai_addr = NULL;
        addrnfo_hints.ai_next = NULL;

        if (0 != (num = getaddrinfo( host
                                     , serv_port
                                     , &addrnfo_hints
                                     , &addrnfo_res
                          ))) {
                fprintf( stderr, "getaddrinfo failed for host '%s', service '%s': '%s'\n"
                         , (host?host:"NULL")
                         , serv_port
                         , gai_strerror(num));
                exit( EXIT_FAILURE );
        }

        // copy to cleanup pointer
        addrnfo_res_save = addrnfo_res;

        do {
                // socket
                if (0 > (listenfd = socket( addrnfo_res->ai_family
                                            , addrnfo_res->ai_socktype
                                            , addrnfo_res->ai_protocol
                                 ))) {
                        perror( "socket failed" );
                        continue; // error try next one
                }

                // set SO_REUSEADDR
                if( 0 > setsockopt( listenfd
                                    , SOL_SOCKET
                                    , SO_REUSEADDR
                                    , &on
                                    , sizeof( on ) ) ){
                        perror("setsockopt failed" );
                }

                // bind
                if (0 == bind( listenfd
                              , addrnfo_res->ai_addr
                              , addrnfo_res->ai_addrlen
                            )) {
                        break; // success
                }
                // bind failed
                perror( "bind failed" );
                close( listenfd );

        } while (NULL != (addrnfo_res->ai_next));

        // check errno from final socket() or bind()
        if (NULL == addrnfo_res) {
                fprintf( stderr, "error for host '%s', service '%s'\n"
                         , (host?host:"NULL")
                         , serv_port
                        );
        }

        // listen
        if (0 > listen( listenfd, LISTENQ )) {
                perror( "listen failed" );
                exit( res );
        }

        // alloc cliaddr
        clilen = addrnfo_res->ai_addrlen;
        if (NULL == (cliaddr = malloc( clilen ) )) {
                perror( "malloc failed" );
                exit( EXIT_FAILURE );
        }

        // freeaddrinfo backup
        /*
          frees the memory that was allocated for the dynamically allocated
          linked list res.
        */
        freeaddrinfo( addrnfo_res_save );

        // signalhandler
        void* setup_signal(int signo, void* func);
        // register function pointer for SIGINT
        if (SIG_ERR == setup_signal( SIGINT, sig_int )) {
                perror( "signal error" );
                free( cliaddr ); cliaddr = NULL;
                exit( EXIT_FAILURE );
        }

        do{
                // accept
        accept_again:
                if (0 > (connfd = accept( listenfd
                                          , cliaddr
                                          , &clilen
                                 ))) {
#ifdef EPROTO
                        if (errno == EPROTO || errno == ECONNABORTED) {
#else
                        if (errno == ECONNABORTED) {
#endif
                                goto accept_again;
                        } else {
                                perror( "accept failed" );
                                res=EXIT_FAILURE;
                                break;
                        }
                }

                // control
                int work_server( int sockfd );
                if (0 > work_server( connfd )) {
                        res=EXIT_FAILURE;
                        break;
                }

                // regular finish
                close( connfd );
                res=EXIT_SUCCESS;

        }while( 0 ); // never loops

        // cleanup
        close( connfd );
        free( cliaddr ); cliaddr=NULL;

        puts("READY.\n");
        exit( res );
}
