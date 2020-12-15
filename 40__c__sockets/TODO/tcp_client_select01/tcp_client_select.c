// tcp_client_select.c
/*
  ipv4 tcp clienet
  multiplexing: select01

  start server (127.0.0.1:12345), or nc
  start this client (127.0.0.1:12345)

  $ ./tcp_client_select.exe
  ping[ENTER]

  ...and server answers
  pong

  ... then closes
  server terminated: Connection reset by peer

  (tcpcli01.c and strcliselect01.c, UNIX Network Programming p.168)
//*/

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
#define BUFSIZE 10

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

/*
// stevens library
static int read_cnt;
static char *read_ptr;
static char read_buf[BUFSIZE];

static ssize_t my_read(int fd, char *ptr)
{

        if (read_cnt <= 0) {
again:
                if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
                        if (errno == EINTR)
                                goto again;
                        return (-1);
                } else if (read_cnt == 0)
                        return (0);
                read_ptr = read_buf;
        }

        read_cnt--;
        *ptr = *read_ptr++;
        return (1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
        ssize_t n, rc;
        char c, *ptr;

        ptr = vptr;
        for (n = 1; n < maxlen; n++) {
                if ((rc = my_read(fd, &c)) == 1) {
                        *ptr++ = c;
                        if (c == '\n')
                                break;        // newline is stored, like fgets()
                } else if (rc == 0) {
                        *ptr = 0;
                        return (n - 1);        // EOF, n - 1 bytes were read
                } else
                        return (-1);        // error, errno set by read()
        }

        *ptr = 0;                // null terminate like fgets()
        return (n);
}
/*/
// lothar's reimplementation
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
        ssize_t cnt = 0;
        char *ptr = NULL;
        int read_cnt = 0;
        char *read_ptr = NULL;
        char read_buf[BUFSIZE]; memset(read_buf, '\0', BUFSIZE);

        ptr = vptr; // cast to char*, 8 bit
        for (cnt = 1; cnt < maxlen; cnt++) {
               if (0 >= read_cnt) {
                      // read a buffer full..
                        do{
                                read_cnt = read(fd, read_buf, sizeof(read_buf));

                                if (0 > read_cnt) {
                                        // error
                                        if (errno == EINTR) continue;
                                        // error, errno set by read()
                                        return (-1);

                                } else if (0 == read_cnt){
                                        // nothing read, EOF
                                        *ptr = 0;
                                        // cnt - 1 bytes were read
                                        return (cnt - 1);

                                } else {
                                        // read from buf into ptr
                                        read_ptr = read_buf;
                                }
                        }while( 0 );
                }
                // work on the buffer read..
                --read_cnt;
                *ptr++ = *read_ptr++;
                if (*ptr == '\n') break; // newline is stored, like fgets()
        }

        *ptr = '\0'; // null terminate like fgets()
        return cnt;
}
//*/

void work_client(int sockfd)
{
        FILE *fp = stdin;
        int maxfdp1 = 0;
        fd_set rset;
        char sendline[BUFSIZE], recvline[BUFSIZE];
        memset(sendline, '\0', BUFSIZE);
        memset(recvline, '\0', BUFSIZE);

        // select - init to zero
        FD_ZERO(&rset);

        while (1) {
                // select - init possible choices
                FD_SET(fileno(fp), &rset);
                FD_SET(sockfd, &rset);

                // select - BLOCKING
                maxfdp1 = 1 + max( fileno(fp), sockfd );
                if (0 > select(maxfdp1, &rset, NULL, NULL, NULL)) {
                        perror("select failed");
                        exit(EXIT_FAILURE);
                }

                // select - choice: answer from socket
                if (FD_ISSET(sockfd, &rset)) {
                        // read from socket
                        if (0 == readline(sockfd, recvline, BUFSIZE)) {
                                perror("server terminated");
                                exit(EXIT_SUCCESS);
                        }

                        // display what was read
                        if (EOF == fputs(recvline, stdout)) {
                                perror("fputs failed");
                                exit(EXIT_FAILURE);
                        }
                        printf("\n");
                }

                // select - choice: input from STDIN
                if (FD_ISSET(fileno(fp), &rset)) {
                        // read from STDIN into fp
                        // only sends when BUFSIZE tokens were read
                        if( NULL == fgets(sendline, BUFSIZE, fp)){
                                if(ferror(fp)){
                                        exit(EXIT_SUCCESS);
                                }
                        }
                        // write fp content to socket (send)
                        writen(sockfd, sendline, strlen(sendline));
                }
        }
}

int main(int argc, char* argv[])
{
        int sockfd;
        struct sockaddr_in servaddr;

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
