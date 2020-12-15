// tcp_client_select.c
/*
  ipv4 tcp client
  multiplexing: select

  STDIN and sockfd are read and multiplexed by select
  reads and sends characters until BUFSIZE on server is full and server reacts
  when something is received from server, this client closes the connection

  server (127.0.0.1:12345)
  this client (127.0.0.1:12345)

  var a:
  provide some input, to be send

  var b:
  simply <ENTER> on input sends server a FIN, to perform a clean shutdown

  (UNIX Network Programming p. 174)
 */

#include <stdlib.h> // exit()
#include <stdio.h> // perror()
#include <string.h> // memset()

#include <sys/socket.h> // socket(), inet_pton()
#include <sys/types.h> // inet_pton()
#include <arpa/inet.h> // sockaddr_in, inet_pton()
#include <errno.h> // errno
#include <unistd.h> // read(), write()
#include <sys/select.h> // select()

#define SERV_PORT 12345
#define SERV_ADDR "127.0.0.1"
#define BUFSIZE 1024

#define max(_a, _b) ((_a>_b)?_a:_b)

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
        FILE *fp = stdin;
        int maxfdp1 = 0, stdineof = 0, num = 0;
        fd_set rset;
        char buf[BUFSIZE]; memset(buf, '\0', BUFSIZE);

        // select - init to zero
        fprintf( stderr, "FD_ZERO(&rset)\n");
        FD_ZERO(&rset);
        while (1) {
                if (stdineof == 0) {
                        // select - register STDIN
                        fprintf( stderr, "FD_SET(fileno(fp), &rset) - and stdineof was 0\n");
                        FD_SET(fileno(fp), &rset);
                }

                // select - register sockfd
                fprintf(stderr, "FD_SET(sockfd, &rset)\n");
                FD_SET(sockfd, &rset);

                // select - BLOCKING, since connected to STDIN, waits on input
                maxfdp1 = 1 + max(fileno(fp), sockfd);
                fprintf( stderr, "SELECT() - file descriptors: fileno(fp) '%d', sockfd '%d', maxfdp1 '%d'\n", fileno(fp), sockfd, maxfdp1);
                if (0 > select(maxfdp1, &rset, NULL, NULL, NULL)) {
                        perror("select failed");
                        exit( EXIT_FAILURE );
                }

                // select - socket ready for reading and writing
                if (FD_ISSET(sockfd, &rset)) {
                        fprintf(stderr, "FD_ISSET(sockfd, &rset) - socket ready for reading and writing\n" );
                        if (0 == (num = read( sockfd, buf, BUFSIZE))) { // network doesn't know '\0'
                                if (stdineof == 1) {
                                        fprintf(stderr, "connection closed by client\n");
                                        return;
                                }else{
                                        perror("connection closed prematurely by server");
                                        exit( EXIT_SUCCESS );
                                }
                        }else if (0 > num) {
                                perror( "read failed" );
                                exit( EXIT_FAILURE );
                        }

                        // display, write to STDOUT
                        writen(fileno(stdout), buf, num);
                        puts("");

                        fprintf( stderr, "done - read '%d' chars from sockfd...\n", num );
                        exit(EXIT_SUCCESS);
                        /*
                          without exit here, the select falls again in FD_ISSET( sockfd,..)
                          since the socked is "ready for read and write" and has the higher
                          fd number (?)
                        //*/
                }


                // select - STDIN ready
                if (FD_ISSET(fileno(fp), &rset)) {
                        fprintf(stderr, "FD_ISSET(fileno(fp), &rset) - STDIN ready\n" );
                        if (0 == (num = read( fileno(fp), buf, BUFSIZE) - 1)) { // STDIN read at least '\0' on <ENTER>
                                fprintf( stderr, "send shutdown to server\n");
                                if (0 > shutdown(sockfd, SHUT_WR)) { // send FIN
                                        perror("shutdown failed");
                                        exit(EXIT_FAILURE);
                                }
                                // select - clear
                                stdineof = 1;
                                fprintf( stderr, "FD_CLR( fileno(fp), &rset) - and stdineof set to 1\n");
                                FD_CLR(fileno(fp), &rset);
                                continue;
                        }

                        // write to socket
                        writen(sockfd, buf, num);
                }
        }
}


int main(int argc, char* argv[])
{
        int sockfd; // connecting fd
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

        // close
        close( sockfd );
        exit( EXIT_SUCCESS );
}
