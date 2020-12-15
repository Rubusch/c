// tcp_server02_meter.c
/*
  resources:
  serv02m.c/child02m.c; UNIX Network Programming, p. XXX

  prefork, no locking; works on BSD-derived systems.
  this version is "metered" to see #clients/child serviced.

  server doesn't need to listen on SIGCHLD of preforked children, since it won't
  continue forking; disadvantage, it doesn't fork anymore, so fixed number of
  children

  meter - an array of longs by the size of nchildren will be allocated, for each
  established connection, a counter is incremented. For disconnections nothing is
  decremented. At shutdown prints the marked connections, that have taken place.

  <valgrind checked>
*/

#include <stdlib.h> // exit(), atoi()
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/types.h> // bind()
#include <sys/socket.h> // socket(), bind(), listen()
#include <arpa/inet.h> // sockaddr
#include <unistd.h> // close(), pause()
#include <errno.h> // errno
#include <signal.h> // SIG_ERR
#include <sys/resource.h> // getrusage()
#include <sys/wait.h> // wait()
#include <sys/mman.h> // mmap()

// struct addrinfo
#define _XOPEN_SOURCE 1
#include <netdb.h> // struct addrinfo, getaddrinfo()

// LISTENQ <= tcp_max_syn_backlog of system <= 1024
#define LISTENQ 10

// debug
//#define MALLOC_CHECK_ 1

// preforked childs
static int nchildren;
static pid_t *pids;
long *cptr=NULL;

void sig_int( int signo )
{
        int idx = -1;
        double user, sys;
        struct rusage myusage, childusage;

        fprintf( stderr, "\t%s:%d - SIGINT caught\n", __FILE__, __LINE__ );
        for (idx=0; idx<nchildren; ++idx) {
                // terminate all children
                kill( pids[idx], SIGTERM );

                // wait on all children
                while (0 < wait(NULL)){
                        ;
                }

                // check for errors
                if (errno != ECHILD) {
                        perror( "wait failed" );
                }
        }

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

        // meter
        for (idx=0; idx<nchildren; ++idx) {
                fprintf( stderr, "child %d, has %ld connection(s)\n", idx, cptr[idx] );
        }

        exit(EXIT_SUCCESS);
}


pid_t child_make( int idx, int listenfd, socklen_t clilen )
{
        pid_t childpid; // fork
        struct sockaddr *cliaddr = NULL; // client address
        int connfd = -1; // socket for captured connection
        int res = EXIT_FAILURE;

        // fork
        if (0 != (childpid = fork())) {
                // parent or error
                return childpid;
        }
        // child
        fprintf( stderr, "\t%s:%d - child %ld starting\n", __FILE__, __LINE__, (long) getpid() );
        extern long *cptr;

        // alloc cliaddr
        if (NULL == (cliaddr = malloc( clilen ))) {
                perror( "malloc failed" );
                kill( getpid(), SIGINT );
        }

        do {
                // accept - after fork(), thus "preforked"
        accept_again:
                if (0 > (connfd = accept( listenfd
                                          , cliaddr
                                          , &clilen
                                 ))) {
#ifdef EPROTO
                        if (errno == EPROTO || errno == ECONNABORTED)
#else
                        if (errno == ECONNABORTED)
#endif
                        {
                                goto accept_again;
                        } else {
                                perror( "accept failed" );
                                res=EXIT_FAILURE;
                                break;
                        }
                }

                // meter pointer
                cptr[idx]++;

                // control
                fprintf( stderr, "\t%s:%d - child %ld has acceptd\n", __FILE__, __LINE__, (long) getpid() );
                int work_server( int sockfd );
                if (0 > work_server( connfd )) {
                        res=EXIT_FAILURE;
                        break;
                }
                res=EXIT_SUCCESS;
                break;

        } while (0); // no loop

        // cleanup
        close( connfd );
        free( cliaddr ); cliaddr = NULL;
        if (0 < cptr[idx]) {
                cptr[idx]--;
        }

        // child exits
        fprintf( stderr, "\t%s:%d - child %ld terminates\n", __FILE__, __LINE__, (long) getpid() );
        if (EXIT_FAILURE == res) {
                kill( getppid(), SIGINT );
        }
        exit( res );
}


/*
  Allocate an array of "nchildren" longs in shared memory that can
  be used as a counter by each child of how many clients it services.
  See pp. 467-470 of "Advanced Programming in the Unix Environment."
*/
long* meter( int nchildren)
{
        long *ptr=NULL;

        // mmap, shared memory
#ifdef MAP_ANONYMOUS
/*
  MAP_ANONYMOUS

  The mapping is not backed by any file; its contents are initialized to zero.
  The fd and offset arguments are ignored; however, some implementations require
  fd to be -1 if MAP_ANONYMOUS (or deprecated MAP_ANON) is specified, and
  portable applications should ensure this. The use of MAP_ANONYMOUS in
  conjunction with MAP_SHARED is only supported on Linux since kernel 2.4.


  MAP_SHARED

  Share this mapping. Updates to the mapping are visible to other processes that
  map this file, and are carried through to the underlying file. The file may
  not actually be updated until msync(2) or munmap() is called.
*/
        // mmap
        if (((void*) -1) == (ptr = mmap( 0
                                         , nchildren * sizeof( long )
                                         , PROT_READ | PROT_WRITE
                                         , MAP_ANONYMOUS | MAP_SHARED
                                         , -1
                                         , 0
                                     ))) {
                perror( "mmap failed" );
                return NULL;
        }
#else
        int fd;

        // mmap
        if (-1 == (fd = open( "/dev/zero", O_RDWR, 0 ))) {
                perror( "open failed" );
                return NULL;
        }

        // open
        if (((void*) -1) == (ptr = mmap( 0
                                         , nchildren * sizeof( long )
                                         , PROT_READ | PROT_WRITE
                                         , MAP_SHARED
                                         , fd
                                         , 0
                                     ))) {
                perror( "mmap failed" );
                return NULL;
        }

        close( fd );
#endif
        return ptr;
}


int main( int argc, char* argv[] )
{
        int listenfd = -1; // socket to listen on
        socklen_t clilen = -1; // length of client address
        char serv_port[16]; memset( serv_port, '\0', 16 ); // service port
        char dev_ip[16]; memset( dev_ip, '\0', 16 ); // optionally specified, device ip
        char *host = NULL;
        struct addrinfo addrnfo_hints; // init object for address info
        struct addrinfo *addrnfo_res = NULL; // result structure for getaddrinfo()
        struct addrinfo *addrnfo_res_save = NULL; // save removal pointer
        const int on=1; // socket options
        int num=-1;
        int idx=-1;

        // CLI args - serv_port, dev_ip, preforked children
        if (argc == 3) {
                fprintf( stderr, "listen on port: '%s'\n", argv[1]);
                strncpy( serv_port, argv[1], 16 );
                serv_port[15]='\0';
        } else if (argc == 4) {
                strncpy( dev_ip, argv[1], 16 );
                dev_ip[15] = '\0';
                host = dev_ip;
                strncpy( serv_port, argv[2], 16 );
                serv_port[15]='\0';
        } else {
                fprintf( stderr, "usage: %s [<dev>] <port> <nchildren>\n", argv[0] );
                exit( EXIT_FAILURE );
        }
        nchildren = atoi(argv[argc-1]);

        // getaddrinfo
        /*
          should return socket addresses for any address family (either IPv4 or
          IPv6, for example) that can be used
        */
        memset( &addrnfo_hints, 0, sizeof( struct addrinfo ) );
        addrnfo_hints.ai_flags = AI_PASSIVE;
        addrnfo_hints.ai_family = AF_UNSPEC;
        addrnfo_hints.ai_socktype = SOCK_STREAM;
        addrnfo_hints.ai_protocol = 0; // any protocol, or issues: ai_socktype
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
                exit( EXIT_FAILURE );
        }

        // alloc child ptrs and prep
        clilen = addrnfo_res->ai_addrlen;
        if (0 > (pids = calloc( nchildren, sizeof( *pids )))) {
                perror( "calloc failed" );
                exit( EXIT_FAILURE );
        }

        // meter
        if (NULL == (cptr = meter( nchildren ))) {
                perror( "meter failed" );
                exit( EXIT_FAILURE );
        }

        // freeaddrinfo backup
        /*
          frees the memory that was allocated for the dynamically allocated
          linked list res.
          must be after addrnfo_res->ai_addrlen was accessed!
        */
        freeaddrinfo( addrnfo_res_save );

        // prefork children
        for (idx=0; idx<nchildren; ++idx) {
                if (0 > (pids[idx] = child_make( idx, listenfd, clilen ))) {
                        perror( "child_make failed, probably fork failed" );
                        free( pids ); pids = NULL;
                        exit( EXIT_FAILURE );
                }
        }

        // signalhandler
        void* setup_signal( int signo, void* func );
        // register function pointer for SIGCHLD and SIGINT
        if (SIG_ERR == setup_signal( SIGINT, sig_int )) {
                perror( "signal error" );
                free( pids ); pids = NULL;
                exit( EXIT_FAILURE );
        }

        do {
                pause(); // everything done by child
        } while (1);

        // cleanup
        free( pids ); pids=NULL;
        puts("READY.\n");
        exit( EXIT_SUCCESS );
}
