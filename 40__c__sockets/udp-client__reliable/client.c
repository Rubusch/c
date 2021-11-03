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
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* alarm() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <arpa/inet.h> /* inet_pton(), inet_ntop() */
#include <setjmp.h>
#include <signal.h> /* signal(), SIG_ERR, SIGALRM, ... */
#include <sys/time.h> /* gettimeofday() */
#include <errno.h>

// constants

#define MAXLINE 4096 /* max text line length */

typedef void Sigfunc(int); /* for signal handlers */

/*
  rtt (round trip time) measuring approach (woud be e.g. in separate a header)

  approach by
  Stevens, 2003
  Jackobson, 1988
*/

//#define RTT_DEBUG
int rtt_d_flag = 0; // debug flag; can be set by caller

struct rtt_info {
	float rtt_rtt; // most recent measured RTT, in seconds
	float rtt_srtt; // smoothed RTT estimator, in seconds
	float rtt_rttvar; // smoothed mean deviation, in seconds
	float rtt_rto; // current RTO to use, in seconds
	int rtt_nrexmt; // # times retransmitted: 0, 1, 2, ...
	uint32_t rtt_base; // # sec since 1/1/1970 at start
};

#define RTT_RXTMIN 2 // min retransmit timeout value, in seconds
#define RTT_RXTMAX 60 // max retransmit timeout value, in seconds
#define RTT_MAXNREXMT 3 // max # times to retransmit

extern int rtt_d_flag; // can be set to nonzero for addl info

// instantiated globals, e.g. in this header
static struct rtt_info rttinfo;
static int rttinit = 0;
static struct msghdr msgsend, msgrecv; // assumed init to 0
static struct hdr {
	uint32_t seq;
	uint32_t ts;
} sendhdr, recvhdr;

// forwards

void err_sys(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);

void lothars__gettimeofday(struct timeval *, void *);
Sigfunc *lothars__signal(int, Sigfunc *);
ssize_t lothars__recvmsg(int, struct msghdr *, int);
void lothars__sendmsg(int, const struct msghdr *, int);
char *lothars__fgets(char *, int, FILE *);
void lothars__fputs(const char *, FILE *);
void lothars__inet_pton(int, const char *, void *);
int lothars__socket(int, int, int);

void rtt_debug(struct rtt_info *);
void rtt_init(struct rtt_info *);
void rtt_newpack(struct rtt_info *);
int rtt_start(struct rtt_info *);
void rtt_stop(struct rtt_info *, uint32_t);
int rtt_timeout(struct rtt_info *);
uint32_t rtt_ts(struct rtt_info *);

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
		snprintf(buf + n_len, MAXLINE - n_len, ": %s",
			 strerror(errno_save));
	}

	strcat(buf, "\n");
	fflush(stdout); // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);
}

// functions

/*
   fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

/*
   nonfatal error unrelated to system call Print message and return
*/
void err_msg(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
}

/*
   fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

static struct sigaction sa;
Sigfunc* lothars__signal(int signo, Sigfunc *func)
{
	// TODO improve - setup handler for initial
	// TODO improve register yet another signal
	sa.sa_handler = func;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (0 > sigaction(signo, &sa, NULL)) {
		err_sys("sigaction error");
		return SIG_ERR;
	}

	return sa.sa_handler;
}

ssize_t lothars__recvmsg(int fd, struct msghdr *msg, int flags)
{
	ssize_t res;
	if (0 > (res = recvmsg(fd, msg, flags))) {
		err_sys("recvmsg error");
	}
	return res;
}

void lothars__sendmsg(int fd, const struct msghdr *msg, int flags)
{
	int idx;
	ssize_t nbytes;

	nbytes = 0; /* must first figure out what return value should be */
	for (idx = 0; idx < msg->msg_iovlen; ++idx) {
		nbytes += msg->msg_iov[idx].iov_len;
	}

	if (nbytes != sendmsg(fd, msg, flags)) {
		err_sys("sendmsg error");
	}
}

void lothars__gettimeofday(struct timeval *tv, void *foo)
{
	if (-1 == gettimeofday(tv, foo)) {
		err_sys("gettimeofday error");
	}
}

char *lothars__fgets(char *ptr, int n, FILE *stream)
{
	char *rptr = NULL;
	if ((NULL == (rptr = fgets(ptr, n, stream))) && ferror(stream)) {
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
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("socket error");
	}
	return res;
}

// rtt implementation

/*
  calculate the RTO value based on current estimators:
  smoothed RTT plus four times the deviation
*/
#define RTT_RTOCALC(ptr) ((ptr)->rtt_srtt + (4.0 * (ptr)->rtt_rttvar))

static float rtt_minmax(float rto)
{
	if (rto < RTT_RXTMIN) {
		rto = RTT_RXTMIN;
	} else if (rto > RTT_RXTMAX) {
		rto = RTT_RXTMAX;
	}

	return rto;
}

void rtt_init(struct rtt_info *ptr)
{
	struct timeval tv;

	lothars__gettimeofday(&tv, NULL);
	ptr->rtt_base = tv.tv_sec; // number of sec since 1/1/1970 at start

	ptr->rtt_rtt = 0;
	ptr->rtt_srtt = 0;
	ptr->rtt_rttvar = 0.75;
	ptr->rtt_rto = rtt_minmax(RTT_RTOCALC(ptr));
	// first RTO at (srtt + (4 * rttvar)) = 3 seconds
}

/*
  return the current timestamp

  our timestamps are 32-bit integers that count milliseconds since
  rtt_init() was called.
*/
uint32_t rtt_ts(struct rtt_info *ptr)
{
	uint32_t ts;
	struct timeval tv;

	lothars__gettimeofday(&tv, NULL);
	ts = ((tv.tv_sec - ptr->rtt_base) * 1000) + (tv.tv_usec / 1000);
	return ts;
}

void rtt_newpack(struct rtt_info *ptr)
{
	ptr->rtt_nrexmt = 0;
}

/*
  return value can be used as: alarm(rtt_start(&foo))
*/
int rtt_start(struct rtt_info *ptr)
{
	return ((int)(ptr->rtt_rto + 0.5)); // round float to int
}

/*
  a response was received

  stop the timer and update the appropriate values in the structure
  based on this packet's RTT.  We calculate the RTT, then update the
  estimators of the RTT and its mean deviation; this function should
  be called right after turning off the timer with alarm(0), or right
  after a timeout occurs
*/
void rtt_stop(struct rtt_info *ptr, uint32_t ms)
{
	double delta;

	ptr->rtt_rtt = ms / 1000.0; // measured RTT in seconds

	/*
	  update our estimators of RTT and mean deviation of RTT
	  see Jacobson's SIGCOMM '88 paper, Appendix A, for the details
	  we use floating point here for simplicity
	*/

	delta = ptr->rtt_rtt - ptr->rtt_srtt;
	ptr->rtt_srtt += delta / 8; // g = 1/8

	if (0.0 > delta) {
		delta = -delta; // |delta|
	}

	ptr->rtt_rttvar += (delta - ptr->rtt_rttvar) / 4; // h = 1/4

	ptr->rtt_rto = rtt_minmax(RTT_RTOCALC(ptr));
}

/*
  a timeout has occurred

  return -1 if it's time to give up, else return 0
*/
int rtt_timeout(struct rtt_info *ptr)
{
	ptr->rtt_rto *= 2; // next RTO

	if (++ptr->rtt_nrexmt > RTT_MAXNREXMT) {
		return -1; // time to give up for this packet
	}
	return 0;
}

/*
  print debugging information on stderr, if the "rtt_d_flag" is nonzero
*/
void rtt_debug(struct rtt_info *ptr)
{
	if (rtt_d_flag == 0) {
		return;
	}

	fprintf(stderr, "rtt = %.3f, srtt = %.3f, rttvar = %.3f, rto = %.3f\n",
		ptr->rtt_rtt, ptr->rtt_srtt, ptr->rtt_rttvar, ptr->rtt_rto);
	fflush(stderr);
}

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
ssize_t costumized_send_recv(int fd, void *outbuff, size_t outbytes,
			     void *inbuff, size_t inbytes,
			     struct sockaddr *destaddr, socklen_t destlen)
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
int main(int argc, char **argv)
{
	int fd_sock;
	struct sockaddr_in servaddr;
	char serverip[16];
	memset(serverip, '\0', sizeof(serverip));
	char port[16];
	memset(port, '\0', sizeof(port));

	if (3 != argc) {
		fprintf(stderr, "usage: %s <serverip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(serverip, argv[1], sizeof(serverip));
	fprintf(stdout, "serverip: '%s'\n", serverip);

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// init server address structure
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));

	// connect the client to the server
	lothars__inet_pton(AF_INET, serverip, &servaddr.sin_addr);

	// get the socket descriptor
	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	// connect the socket to the command line console
	{
		ssize_t n_bytes;
		struct sockaddr *pservaddr = (struct sockaddr *)&servaddr;

		// received line has additionally '\0'
		char sendline[MAXLINE], recvline[MAXLINE + 1];

		while (NULL != lothars__fgets(sendline, MAXLINE, stdin)) {
			n_bytes = costumized_send_recv(
				fd_sock, sendline, strlen(sendline), recvline,
				MAXLINE, pservaddr, sizeof(servaddr));

			recvline[n_bytes] = '\0'; // null terminate

			lothars__fputs(recvline, stdout);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
