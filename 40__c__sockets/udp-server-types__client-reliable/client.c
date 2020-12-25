// client.c
/*
  reliable UDP echo client

  contains:
  - timeout
  - retransmit
  - sequence numbers


  Concept how to add reliability (inspired by Stevens 2003):
  (rtt = round trip time measuring)

    static sigjmp_buf jmpbuf;
    {
      ...
      from request
      ...

      // establish signal handler
      signal(SIGALRM, sig_alarm);

      // init re-transmit counter to 0
      rtt_newpack();

      sendagain:
        sentdo();

      // set alarm for RTO seconds
      alarm(rtt_start());

      if(sigsetjmp(jmpbuf, 1) != 0){

        // double RTO, retransmitted enough?!
	if(rtt_timeout()){
          // give up
	  goto sendagain; // retransmit
        }
      }

      do {
        recvfrom();
      } while ( wrong sequence number );

      // turn off alarm
      alarm(0);

      // calculate RTT and update estimators
      rtt_stop();

      process reply
      ...
    }

    void sig_alarm(int signo){ siglongjmp(jmpbuf, 1); }
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> /* va_start(), va_end(),... */
#include <arpa/inet.h> /* inet_pton(), inet_ntop() */
#include <setjmp.h>
#include <errno.h>


// constants

#define MAXLINE  4096 /* max text line length */


// forwards

// TODO resolve from lib snippets         
// rtt_newpack()
// rtt_stop()
// rtt_timeout()
// rtt_ts()
// rtt_init()

// lothars__signal()
// lothars__sendmsg()
// lothars__recvmsg()
char* lothars__fgets(char *, int, FILE *);
void lothars__fputs(const char *, FILE *);
void lothars__inet_pton(int, const char*, void*);
int lothars__socket(int, int, int);


// utilities

/*
   Print message and return to caller Caller specifies "errnoflag"
*/
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save, n_len;
	char buf[MAXLINE + 1];

	errno_save = errno; // value caller might want printed

	vsnprintf(buf, MAXLINE, fmt, ap); // safe
//	vsprintf(buf, fmt, ap); // alternatively, if no vsnprintf() available, not safe

	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");
	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);
}


// functions

/*
   Fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


/*
   Fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


char* lothars__fgets(char *ptr, int n, FILE *stream)
{
	char *rptr = NULL;
	if ( (NULL == (rptr = fgets(ptr, n, stream))) && ferror(stream)) {
		err_sys("fgets error");
	}
	return rptr;
}


void lothars__fputs(const char *ptr, FILE *stream)
{
	if (EOF == fputs(ptr, stream)) {
		err_sys("fputs error");
	}
}


void lothars__inet_pton(int family, const char *strptr, void *addrptr)
{
	int res;
	if (0 > (res = inet_pton(family, strptr, addrptr))) {
		err_sys("inet_pton error for %s", strptr); // errno set
	} else if (0 == res) {
		err_quit("inet_pton error for %s", strptr); // errno not set
	}
}


int lothars__socket(int family, int type, int protocol)
{
	int res;
	if(0 > (res = socket(family, type, protocol))){
		err_sys("socket error");
	}
	return res;
}


//#define RTT_DEBUG

static struct rtt_info rttinfo;
static int rttinit = 0;
static struct msghdr msgsend, msgrecv; // assumed init to 0
static struct hdr {
	uint32_t seq;
	uint32_t ts;
} sendhdr, recvhdr;


/*
  mini signal handler
*/
static sigjmp_buf jmpbuf;
static void sig_alarm(int signo)
{
	siglongjmp(jmpbuf, 1);
}


/*
  the reliable io - reliability added to send / receive
*/
ssize_t costumized_send_recv( int fd
                              , void* outbuff
                              , size_t outbytes
                              , void* inbuff
                              , size_t inbytes
                              , struct sockaddr* destaddr
                              , socklen_t destlen)
{
	ssize_t num;
	struct iovec iovsend[2], iovrecv[2];

	if (rttinit == 0) {
		// first time we're called
		rtt_init(&rttinfo);
		rttinit = 1;
		rtt_d_flag = 1;
	}

	// init globals
	++sendhdr.seq;

	msgsend.msg_name = destaddr;
	msgsend.msg_namelen = destlen;
	msgsend.msg_iov = iovsend;
	msgsend.msg_iovlen = 2;

	iovsend[0].iov_base = &sendhdr;
	iovsend[0].iov_len = sizeof(struct hdr);
	iovsend[1].iov_base = outbuff;
	iovsend[1].iov_len = outbytes;

	msgrecv.msg_name = NULL;
	msgrecv.msg_namelen = 0;
	msgrecv.msg_iov = iovrecv;
	msgrecv.msg_iovlen = 2;

	iovrecv[0].iov_base = &recvhdr;
	iovrecv[0].iov_len = sizeof(struct hdr);
	iovrecv[1].iov_base = inbuff;
	iovrecv[1].iov_len = inbytes;

	lothars__signal(SIGALRM, sig_alarm);

	// initialize for this packet
	rtt_newpack(&rttinfo);

sendagain:
	sendhdr.ts = rtt_ts(&rttinfo);

	// send
	lothars__sendmsg(fd, &msgsend, 0);

	// calc timeout value & start timer
	alarm(rtt_start(&rttinfo));

	if (sigsetjmp(jmpbuf, 1) != 0) {
		if (0 > rtt_timeout(&rttinfo)) {
			err_msg("costumized_send_recv: no response from server, giving up");
			rttinit = 0;
			errno = ETIMEDOUT;
			return -1;
		}
		goto sendagain;
	}

	do {
		num = lothars__recvmsg(fd, &msgrecv, 0);
	} while (num < sizeof(struct hdr) || recvhdr.seq != sendhdr.seq);

	// stop SIGALRM timer
	alarm(0);

	// calculate & store new RTT estimator values
	rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);

	// return size of received datagram
	return num - sizeof(struct hdr);
}


/* TODO rm       
  helper to call the "reliable" send / receive

void costumized_dg_cli( FILE* fp
                        , int fd_sock
                        , SA* pservaddr
                        , socklen_t servlen)
{
	ssize_t num;

	// received line has additionally '\0'
	char sendline[MAXLINE], recvline[MAXLINE + 1];

	while (NULL != lothars__fgets(sendline, MAXLINE, fp)){
		num = costumized_send_recv( fd_sock
					    , sendline
					    , strlen(sendline)
					    , recvline
					    , MAXLINE
					    , pservaddr
					    , servlen);

		recvline[num] = '\0'; // null terminate

		lothars__fputs(recvline, stdout);
	}
}   
// */


/*
  main()...

  UDP client function
*/
int main(int argc, char** argv)
{
	int fd_sock;
	struct sockaddr_in servaddr;

	// init server address structure
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	// connect the client to the server on ip = 10.0.2.2
	lothars__inet_pton(AF_INET, "10.0.2.2", &servaddr.sin_addr);

	// get the socket descriptor
	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	// connect the socket to the command line console
	
//	costumized_dg_cli(stdin, fd_sock, (struct sockaddr*) &servaddr, sizeof(servaddr));	// TODO replace costumized_dg_cli()             
//void costumized_dg_cli( FILE* fp
//                        , int fd_sock
//                        , SA* pservaddr
//                        , socklen_t servlen)
	{
		ssize_t n_bytes;
		struct sockaddr* pservaddr = (struct sockaddr*) &servaddr;

		// received line has additionally '\0'
		char sendline[MAXLINE], recvline[MAXLINE + 1];

		while (NULL != lothars__fgets(sendline, MAXLINE, stdin)){
			n_bytes = costumized_send_recv( fd_sock
						    , sendline
						    , strlen(sendline)
						    , recvline
						    , MAXLINE
						    , pservaddr
						    , sizeof(servaddr));

			recvline[n_bytes] = '\0'; // null terminate

			lothars__fputs(recvline, stdout);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
