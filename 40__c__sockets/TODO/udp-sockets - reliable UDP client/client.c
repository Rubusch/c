// client.c
/*
  reliable UDP echo client

  contains:
  - timeout
  - retransmit
  - sequence numbers


  Concept how to add reliability (Stevens 2003):  
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
  
    do{
      recvfrom();
    }while( wrong sequence number );

    // turn off alarm
    alarm(0);

    // calculate RTT and update estimators
    rtt_stop();
    
    process reply
    ...
  }

  void sig_alarm(int signo){ siglongjmp(jmpbuf, 1); }
//*/



#include "../lib_socket/lib_socket.h"
#include "../lib_socket/lib_rtt/rtt.h" 

#include <setjmp.h>


#define RTT_DEBUG

static struct rtt_info rttinfo;
static int rttinit = 0;
static struct msghdr msgsend, msgrecv; // assumed init to 0
static struct hdr{
  uint32_t seq;
  uint32_t ts;
}sendhdr, recvhdr;


/*
  mini signal handler
//*/
static sigjmp_buf jmpbuf;
static void sig_alarm(int signo)
{
  siglongjmp(jmpbuf, 1);
}


/*
  the reliable io - reliability added to send / receive
//*/
ssize_t costumized_send_recv( int fd
                              , void* outbuff
                              , size_t outbytes
                              , void* inbuff
                              , size_t inbytes
                              , SA* destaddr
                              , socklen_t destlen)
{
  ssize_t num;
  struct iovec iovsend[2], iovrecv[2];

  if(rttinit == 0){
    // first time we're called
    rtt_init(&rttinfo);
    rttinit = 1;
    rtt_d_flag = 1;
  }

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

  _signal(SIGALRM, sig_alarm);
  
  // initialize for this packet
  rtt_newpack(&rttinfo);
  
 sendagain:
  sendhdr.ts = rtt_ts(&rttinfo);
  _sendmsg(fd, &msgsend, 0);
  
  // calc timeout value & start timer
  alarm(rtt_start(&rttinfo)); 
  
  if(sigsetjmp(jmpbuf, 1) != 0){
    if(0 > rtt_timeout(&rttinfo)){
      err_msg("costumized_send_recv: no response from server, giving up");
      rttinit = 0;
      errno = ETIMEDOUT;
      return -1;
    }
    goto sendagain;
  }

  do{
    num = _recvmsg(fd, &msgrecv, 0);
  }while(num < sizeof(struct hdr) || recvhdr.seq != sendhdr.seq);

  // stop SIGALRM timer
  alarm(0);
  
  // calculate & store new RTT estimator values
  rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);
  
  // return size of received datagram
  return num - sizeof(struct hdr);
}


/*
  helper to call the "reliable" send / receive
//*/
void costumized_dg_cli( FILE* fp
                        , int fd_sock
                        , SA* pservaddr
                        , socklen_t servlen)
{
  ssize_t num;  
  
  // received line has additionally '\0'
  char sendline[MAXLINE], recvline[MAXLINE + 1];

  while(NULL != _fgets(sendline, MAXLINE, fp)){
    num = costumized_send_recv( fd_sock
                                , sendline
                                , strlen(sendline)
                                , recvline
                                , MAXLINE
                                , pservaddr
                                , servlen);
    
    recvline[num] = '\0'; // null terminate
  
    _fputs(recvline, stdout);
  }
}


/*
  main UDP client function
//*/
int main(int argc, char** argv)
{
  int fd_sock;
  struct sockaddr_in servaddr;

  // init server address structure
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);

  // connect the client to the server on ip = 10.0.2.2
  _inet_pton(AF_INET, "10.0.2.2", &servaddr.sin_addr);

  // get the socket descriptor
  fd_sock = _socket(AF_INET, SOCK_DGRAM, 0);

  // connect the socket to the command line console
  costumized_dg_cli(stdin, fd_sock, (SA*) &servaddr, sizeof(servaddr));
  
  exit(0);
}
