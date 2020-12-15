// tcp_server05.c
/*
  resources:
  serv05.c; UNIX Network Programming, p. XXX


  serv05: prefork, descrptor passing to children.  Similar to NSCA server.

  my_lock_init() sets up a lock file under /tmp
  my_lock_wait() locks before accept()
  my_lock_release() unlocks after accept()

  <valgrind checked>
*/

#include <stdlib.h> // exit(), atoi()
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/types.h> // bind()
#include <sys/socket.h> // socket(), bind(), listen(), socketpair()
#include <arpa/inet.h> // sockaddr
#include <unistd.h> // close(), pause(), dup2()
#include <errno.h> // errno
#include <signal.h> // SIG_ERR
#include <sys/resource.h> // getrusage()
#include <sys/wait.h> // wait()

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

typedef struct {
        pid_t child_pid; // process id
        int child_pipefd; // parent's stream pipe to/from child
        int child_status; // 0 = ready
        long child_count; // # connections handled
} child_t;

// array of child_t structures, calloc'ed
child_t *cptr;


void sig_int( int signo )
{
        int idx = -1;
        double user, sys;
        struct rusage myusage, childusage;

        fprintf( stderr, "\t%s:%d - SIGINT caught\n", __FILE__, __LINE__ );
        for (idx=0; idx<nchildren; ++idx) {
                // terminate all children
                kill( child[idx].child_pid, SIGTERM );

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

        for (idx=0; idx<nchildren; ++idx) {
                fprintf( stderr, "\t%s:%d - child '%d', '%ld' connections\n"
                         , __FILE__, __LINE__, idx, cptr[idx].child_count );
        }

        exit(EXIT_SUCCESS);
}


pid_t child_make( int idx, int listenfd, socklen_t clilen )
{
        int sockfd[2]; memset(sockfd, 0, sizeof(sockfd));
        pid_t childpid; // fork
//        int res = EXIT_FAILURE; // TODO

        if (0 > socketpair( AF_LOCAL, SOCK_STREAM, 0, sockfd )) {
                perror( "socketpair failed" );
                exit( EXIT_FAILURE );
        }

        if (0 > (childpid = fork())) {
                perror("fork failed");
                exit( EXIT_FAILURE );

        } else if (0 < childpid) {
                // parent
                if (0 > close( sockfd[1] )) {
                        perror( "close failed for parent" );
                        exit( EXIT_FAILURE );
                }
                cptr[idx].child_pid = childpid;
                cptr[idx].child_pipefd = sockfd[0];
                cptr[idx].child_status = 0;
                return childpid;
        }

        // child
        // child's stream pipe to parent
        /*
          duplicate sockfd[1] data descriptor to STDERR_FILENO
// TODO what is happening here? which IPC mech?
         */
        if (0 > dup2( sockfd[1], STDERR_FILENO )) {
                perror( "dup2 failed" );
                exit( EXIT_FAILURE );
        }

        if (0 > close( sockfd[0] )) {
                perror( "close failed for sockfd[0]" );
                exit( EXIT_FAILURE );
        }
        if (0 > close( sockfd[1] )) {
                perror( "close failed for sockfd[1]" );
                exit( EXIT_FAILURE );
        }
        if (0 > close( listenfd )) { // child does not need this open
                perror( "close failed for listenfd" );
                exit( EXIT_FAILURE );
        }

        char chr = '\0';
        int connfd = -1;
        size_t num;
//        int connfd = -1; // socket for captured connection
//        struct sockaddr *cliaddr = NULL; // client address
        fprintf( stderr, "\t%s:%d - child %ld starting\n", __FILE__, __LINE__, (long) getpid() );

        
/*
        // alloc cliaddr
        if (NULL == (cliaddr = malloc( clilen ))) {
                perror( "malloc failed" );
                kill( getpid(), SIGINT );
        }
//*/
        do {
// TODO
                if (0 == (n = read_fd( STDERR_FILENO, &chr, 1, &connfd ))) {    
                        perror( "read_fd failed" );    
                        exit( EXIT_FAILURE );
                }

// TODO
                if (0 > connfd) {
                        perror( "connfd failed" );
                        exit( EXIT_FAILURE );
                }

                 
/*
                // lock
                void my_lock_wait();
                my_lock_wait();

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

                // unlock
                void my_lock_release();
                my_lock_release();
//*/

                // control
                fprintf( stderr, "\t%s:%d - child %ld has acceptd\n", __FILE__, __LINE__, (long) getpid() );
                int work_server( int sockfd );
                if (0 > work_server( connfd )) {
                        res=EXIT_FAILURE;
                        break;
                }
                res=EXIT_SUCCESS;
                break;

                // close
                if (0 > close( connfd )) {
                        perror( "close failed for child" );
                        exit( EXIT_FAILURE );
                }

                // Write - tell parent we're ready again
// TODO
                Write(STDERR_FILENO, "", 1);


//        free( cliaddr ); cliaddr = NULL; // TODO  

        } while (1);

/* // TODO
        // child exits
        fprintf( stderr, "\t%s:%d - child %ld terminates\n", __FILE__, __LINE__, (long) getpid() );
        if (EXIT_FAILURE == res) {
                kill( getppid(), SIGINT );
        }
        exit( res );
*/
}


// TODO merge serv05.c                                                                                  


int main( int argc, char* argv[] )
{
        int listenfd = -1; // socket to listen on
        socklen_t addrlen, clilen = -1; // length of client address // TODO rm    
        // TODO check clilen is addrlen in make_child()   
        char serv_port[16]; memset( serv_port, '\0', 16 ); // service port
        char dev_ip[16]; memset( dev_ip, '\0', 16 ); // optionally specified, device ip
        char *host = NULL;
        struct addrinfo addrnfo_hints; // init object for address info
        struct addrinfo *addrnfo_res = NULL; // result structure for getaddrinfo()
        struct addrinfo *addrnfo_res_save = NULL; // save removal pointer
        const int on=1; // socket options
        int num=-1;
        int idx=-1;

// TODO
        int navail, maxfd, nsel, connfd, rc;
// TODO
//        size_t num    
        fd_set rset, masterset;
        sockaddr *cliaddr;
        

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

        // freeaddrinfo backup
        /*
          frees the memory that was allocated for the dynamically allocated
          linked list res.
          must be after addrnfo_res->ai_addrlen was accessed!
        */
        freeaddrinfo( addrnfo_res_save );

        // select - init
        FD_ZERO( &masterset );
        FD_SET( listenfd, &masterset );
        maxfd = listenfd;

        // alloc cliaddr
        if (NULL == (cliaddr = malloc( clilen ))) {
                perror( "malloc failed" );
                exit( EXIT_FAILURE );
        }

        // available children
        navail = nchildren;

        // alloc cptr[]
        if (NULL > (cptr = calloc( nchildren, sizeof( *cptr )))) {
                perror( "calloc failed" );
                exit( EXIT_FAILURE );
        }

        // prep all children
        for (idx=0; idx < nchildren; ++idx) {
                child_make( idx, listenfd, addrlen ); // parent returns
                FD_SET( cptr[idx].child_pipefd, &masterset );
                maxfd = max( maxfd, cptr[idx].child_pipefd );
        }
                
/*
        if (0 > (pids = calloc( nchildren, sizeof( *pids )))) {
                perror( "calloc failed" );
                exit( EXIT_FAILURE );
        }

        // freeaddrinfo backup
        // *
        //  frees the memory that was allocated for the dynamically allocated
        //  linked list res.
        //  must be after addrnfo_res->ai_addrlen was accessed!
        //* /
        freeaddrinfo( addrnfo_res_save );

        // lock, one lock file for all children
        void my_lock_init( const char* );
        my_lock_init( "/tmp/lock.XXXXXX" ); // name must contain 'XXXXXX'

        // prefork children
        for (idx=0; idx<nchildren; ++idx) {
                if (0 > (pids[idx] = child_make( idx, listenfd, clilen ))) {
                        perror( "child_make failed, probably fork failed" );
                        free( pids ); pids = NULL;
                        exit( EXIT_FAILURE );
                }
        }
//*/

        // signalhandler
        void* setup_signal( int signo, void* func );
        // register function pointer for SIGCHLD and SIGINT
        if (SIG_ERR == setup_signal( SIGINT, sig_int )) {
                perror( "signal error" );
                free( pids ); pids = NULL;
                exit( EXIT_FAILURE );
        }
        
        do {
                rset = masterset;

                if (0 >= navail) {
                        FD_CLR( listenfd, &rset ); // turn off if no available children
                }

                if (0 > (nsel = select( listenfd + 1
                                        , &rset
                                        , NULL
                                        , NULL
                                        , NULL
                                 ))) {
                        perror( "select failed" );
                        kill( getpid(), SIGINT );
                }

                // new connections
                if (FD_ISSET( listenfd, &rset )) {
                        clilen = addrlen;

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

                        // find idle child
                        for (idx=0; idx<nchildren; ++idx) {
                                if (0 == child[idx].child_status) {
                                        break;
                                }
                        }
                        if (idx == nchildren) {
                                perror( "no available children" );
                                exit( EXIT_FAILURE );
                        }

                        cptr[idx].child_status = 1; // mark as busy
                        ++(cptr[idx].child_count);
                        --navail;
                
// TODO
                        num = Write_fd( cptr[idx].child_pipefd, "", 1, connfd); // TODO           
                        if (0 > close( connfd )) {
                                perror( "close connfd failed" );
                                exit( EXIT_FAILURE );
                        }

                        // TODO
                        if (0 == nsel) {
                                continue;
                        }
                }

                // any newly available children?
                for (idx=0; idx<nchildren; ++idx) {
                        if (FD_ISSET( child[idx].child_pipefd, &rset)) {
// TODO
                                if (0 == (num = Read_fd( child[idx].child_pipefd, &rc, 1))) {    
                                        fprintf( stderr, "\t%s:%d - child '%d' terminated unexpectedly\n", __FILE__, __LINE__, idx );
                                        kill( getpid(), SIGINT );
                                }

                                cptr[idx].child_status = 0;
                                ++navail;
                                if (--nsel == 0) {
                                        break; // all done with select results
                                }
                }
// TODO rm
//                pause(); // everything done by child
        } while (1);

        // cleanup
        free( pids ); pids=NULL;
        puts("READY.\n");
        exit( EXIT_SUCCESS );
}
