// ping.c
/*
  raw socket implementation of ping
//*/

#include "ping.h"


/*
  IPv4 handler

  ICMP protocol:
   
  |                             len                             |
  +------------------------------+------------------------------+
  |             hlen1            |           icmplen            |
  +---------------+--------------+--------------+---------------+
  | IPv4 header   | IPv4 options | ICMPv4       | ICMP          |
  +---------------+--------------+--------------+---------------+
  | 20 bytes      | 0 - 40 bytes | 8 bytes                      |
  +---------------+--------------+------------------------------+
  
//*/
void proc_v4(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tv_recv)
{
  int32_t hlen1, icmplen;
  double rtt; // round trip time
  struct ip *ip=NULL;
  struct icmp *icmp=NULL;
  struct timeval *tv_send=NULL;

  // init with start of IP header
  ip = (struct ip*) ptr;

  // length of IP header
  hlen1 = ip->ip_hl << 2;
  if(ip->ip_p != IPPROTO_ICMP){
    // no ICMP protocol
    return;
  }

  // init with start of ICMP header
  icmp = (struct icmp*) (ptr + hlen1);
  if(8 > (icmplen = len - hlen1)){
    // malformed packet
    return;
  }

  if(ICMP_ECHOREPLY == icmp->icmp_type){

    if(pid != icmp->icmp_id){
      // not a response to our ECHO_REQUEST
      return;
    }

    if(16 > icmplen){
      // not enough data to use
      return;
    }

    tv_send = (struct timeval*) icmp->icmp_data;
    tv_sub(tv_recv, tv_send);
    
    // calculate round trip time
    rtt = tv_recv->tv_sec * 1000.0 + tv_recv->tv_usec / 1000.0;

    printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n"
           , icmplen
           , _sock_ntop_host(proto->sa_recv, proto->salen)
           , icmp->icmp_seq
           , ip->ip_ttl
           , rtt); 

  }else if(verbose){
    printf(" %d bytes from %s: type=%d, code=%d\n"
           , icmplen
           , _sock_ntop_host(proto->sa_recv, proto->salen)
           , icmp->icmp_type
           , icmp->icmp_code);
  }
}


/*
  init IPv6 handler
//*/
void init_v6()
{
#ifdef IPV6
  const int8_t on=1;
  
  if(0 == verbose){
    // install a filter that only passes ICMP6_ECHO_REPLY unless verbose
    struct icmp6_filter v6_filter;
    ICMP6_FILTER_SETBLOCKALL(&v6_filter);
    ICMP6_FILTER_SETPASS(ICMP6_ECHO_REPLY, &v6_filter);
    setsockopt(fd_sock, IPPROTO_IPV6, ICMP6_FILTER, &v6_filter, sizeof(v6_filter));
    // ignore error return, the filter is an optimization
  }
  
  // ignore error returned below, we just won't receive the hop limit
# ifdef IPV6_RECVHOPLIMIT
  // RFC 3542
  setsockopt(fd_sock, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on));
# else
  // RFC 2292
  setsockopt(fd_sock, IPPROTO_IPV6, IPV6_HOPLIMIT, &on, sizeof(on));
# endif
#endif  
}


/*
  IPv6 handler
//*/
void proc_v6(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tv_recv)
{
#ifdef IPV6
  double rtt; // round trip time
  struct icmp6_hdr *icmp6=NULL;
  struct timeval *tv_send=NULL;
  struct cmsghdr *cmsg=NULL;
  int32_t hlim;
  
  icmp6 = (struct icmp6_hdr*) ptr;
  
  if(8 > len){
    // malformed packet
    return;
  }

  if(ICMP6_ECHO_REPLY == icmp6->icmp6_type){
    if(pid != icmp6->icmp6_id){
      // not a response to our ECHO_REQUEST
      return;
    }

    if(16 > len){
      // not enough data to use
      return;
    }

    tv_send = (struct timeval*) (icmp6 + 1);
    tv_sub(tv_recv, tv_send);
    rtt = tv_recv->tv_sec * 1000.0 + tv_recv->tv_usec / 1000.0;
    
    hlim = -1;
    for(cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL; cmsg = CMSG_NXTHDR(msg, cmsg)){
      if((cmsg->cmsg_level == IPPROTO_IPV6) && (cmsg->cmsg_type == IPV6_HOPLIMIT)){
        hlim = *(u_int32_t*) CMSG_DATA(cmsg);
        break;
      }
    }

    printf("%d bytes from %s: seq=%u, hlim="
           , len
           , _sock_ntop_host(proto->sa_recv, proto->salen)
           , icmp6->icmp6_seq);
    
    if(-1 == hlim){
      // ancilliary data missing
      printf("???"); 
    }else{
      printf("%d", hlim);
    }

    printf(", rtt=%.3f ms\n", rtt);

  }else if(verbose){
    printf(" %d bytes from %s: type=%d, code=%d\n"
           , len
           , _sock_ntop_host(proto->sa_recv, proto->salen)
           , icmp6->icmp6_type
           , icmp6->icmp6_code);
    
  }    
#endif
}


/*
  send - IPv4
//*/
void send_v4()
{
  int32_t len;
  struct icmp *icmp=NULL;
  
  icmp = (struct icmp*) sendbuf;
  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_id = pid;
  icmp->icmp_seq = ++n_sent; // sequence numbers..

  // fill with pattern
  memset(icmp->icmp_data, 0xa5, datalen); 
  
  _gettimeofday((struct timeval*) icmp->icmp_data, NULL);

  // checksum ICMP header and data
  len = 8 + datalen;
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = in_cksum((u_int16_t*) icmp, len);
  
  _sendto(fd_sock, sendbuf, len, 0, proto->sa_send, proto->salen);
}


/*
  calculate checksum
//*/
uint16_t in_cksum(uint16_t *addr, int32_t len)
{
  register int32_t n_left = len;
  register uint32_t sum = 0;
  register uint16_t *word = addr;
  uint16_t answer = 0;
  
  // our algorithm is simple, using a 32 bit accumulator (sum), we add
  // sequential 16 bit words to it, and at the end, fold back all the 
  // carry bits from the top 16 bits into the lower 16 bits
  while(1 < n_left){
    sum += *word++; 
    /*
     "*word++" is C's fetch-and-advance:
     1. step
     sum += *word;
     
     2. step
     ++word;
    //*/
    n_left -= 2;
  }
  
  // mop up an odd byte, if necessary
  if(1 == n_left){
    *(unsigned char*) (&answer) = *(unsigned char*) word;
    sum += answer;
  }

  // add back carry outs from top 16 bits to low 16 bits

  // add hi 16 to low 16
  sum = (sum >> 16) + (sum & 0xffff);
  
  // add carry
  sum += (sum >> 16);
  
  // truncate to 16 bits
  answer = ~sum;
  
  return answer;
}


/*
  send - IPv6
//*/
void send_v6()
{
#ifdef IPV6
  int32_t len;
  struct icmp6_hdr *icmp6 = NULL;
  
  icmp6 = (struct icmp6_hdr*) sendbuf;
  icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
  icmp6->icmp6_code = 0;
  icmp6->icmp6_id = pid;
  icmp6->icmp6_seq = ++n_sent;

  // fill with pattern
  memset( (icmp6 + 1), 0xa5, datalen);
  _gettimeofday((struct timeval*) (icmp6 + 1), NULL);

  // 8-byte ICMPv6 header
  len = 8 + datalen;

  _sendto(fd_sock, sendbuf, len, 0, proto->sa_send, proto->salen);
  
  // kernel claculates and stores checksum for us

#endif
}


/*
  read loop
//*/
void readloop()
{
  int32_t size;

  // the receiving buffer
  char recvbuf[BUFSIZE];

  char controlbuf[BUFSIZE];
  struct msghdr msg;
  struct iovec iov;
  ssize_t cnt;
  struct timeval tval;

  // create raw socket, using the specified protocol family
  fd_sock = _socket(proto->sa_send->sa_family, SOCK_RAW, proto->icmp_proto);

  // don't need special permissions any more
  setuid(getuid());
  
  if(proto->finit){
    (*proto->finit) ();
  }

  // OK if setsockopt fails..
  size = 60 * 1024;
  setsockopt(fd_sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

  // send first packet
  sig_alrm(SIGALRM);

  // reading 1: set the iov pointer on the address of the "receiving buffer",
  // in order to read the stream into the "receiving buffer"
  iov.iov_base = recvbuf;
  iov.iov_len = sizeof(recvbuf);
  
  msg.msg_name = proto->sa_recv;

  // reading 2: setting the io vector's address to the message header structure
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = controlbuf;
  
  while(1){
    msg.msg_namelen = proto->salen;
    msg.msg_controllen = sizeof(controlbuf);

    // reading 3: actual reading in-loop, writing into the "msg" structure's io vector
    if(0 > (cnt = recvmsg(fd_sock, &msg, 0))){
      if(errno == EINTR){
        // only an interrupt was caught..
        continue;
      }else{
        err_sys("recvmsg error");
      }
    }

    _gettimeofday(&tval, NULL);

    // reading 4: parsing the information in "fproc" function...
    // call "procedure" function pointer with arguments (within while loop!)
    (*proto->fproc) (recvbuf, cnt, &msg, &tval);
  }
}


/*
  mini signal handler

  sends packets actually timed in 1 sec intervals
//*/
void sig_alrm(int32_t signo)
{
  (*proto->fsend) ();
  alarm(1);
}


/*
  init globally the used protocol structures
//*/
struct protocol proto_v4 = { proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP };
#ifdef IPV6
struct protocol proto_v6 = { proc_v6, send_v6, init_v6, NULL, NULL, 0, IPPROTO_ICMPV6 };
#endif

// data that goes with ICMP echo request
int32_t datalen = 56; 

/*
  ping - main
//*/
int main(int argc, char** argv)
{
  int32_t cnt;
  struct addrinfo *ai=NULL;
  char *pchr=NULL;
  
  // don't want getopt() writing to stderr
  opterr = 0;

  while(-1 != (cnt = getopt(argc, argv, "v"))){
    switch(cnt){
    case 'v':
      ++verbose;
      break;
      
    case '?':
      err_quit("unrecognized option: %c", cnt);
    }
  }

  if(optind != argc -1){
    err_quit("usage: ping [ -v ] <hostname>");
  }
  host = argv[optind];
  
  // ICMP ID field is 16 bits
  pid = getpid() & 0xffff;

  // set signal handler - to actually SEND the packets
  _signal(SIGALRM, sig_alrm);

  // set address information (==ai)
  ai = _host_serv(host, NULL, 0, 0);

  pchr = _sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
  printf("PING %s (%s): %d data bytes\n"
         , (ai->ai_canonname ? ai->ai_canonname : pchr)
         , pchr
         , datalen);

  // initialize sending pointer "proto" according to the used protocol
  if(ai->ai_family == AF_INET){
    proto = &proto_v4;
#ifdef IPV6
  }else if(ai->ai_family == AF_INET6){
    proto = &proto_v6;
    if(IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6*) ai->ai_addr)->sin6_addr))){
      err_quit("cannot ping IPv4-mapped IPv6 address");
    }
#endif
  }else{
    err_quit("unknown address family %d", ai->ai_family);
  }

  // start prg
  proto->sa_send = ai->ai_addr;
  proto->sa_recv = _calloc(1, ai->ai_addrlen);
  proto->salen = ai->ai_addrlen;
  
  readloop();

  exit(0);
}

