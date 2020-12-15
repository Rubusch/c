// traceroute.c
/*
  FIXME: no diference in options -m and -v -> check
  FIXME: comment
//*/

#include "traceroute.h"


/*
  set flag to note that alarm occured and interrupt the recvfrom 
//*/
void sig_alrm(int signo)
{
  gotalarm = 1; 
}


/*
  TODO
//*/
void traceloop()
{
  int seq, code, done;
  double rtt;
  struct rec *rec=NULL;
  struct timeval tv_recv;

  // generate raw socket
  fd_recv = _socket(proto->sa_send->sa_family, SOCK_RAW, proto->icmp_proto);
  
  // don't need special permissions anymore
  setuid(getuid());

#ifdef IPV6
  if((proto->sa_send->sa_family == AF_INET6) && (verbose == 0)){
    struct icmp6_filter v6_filter;
    ICMP6_FILTER_SETBLOCKALL(&v6_filter);
    ICMP6_FILTER_SETPASS(ICMP6_TIME_EXCEEDED, &v6_filter);
    ICMP6_FILTER_SETPASS(ICMP6_DST_UNREACH, &v6_filter);
    setsockopt(fd_recv, IPPROTO_IPV6, ICMP6_FILTER, &v6_filter, sizeof(v6_filter));
  }
#endif
  
  // create UDP socket
  fd_send = _socket(proto->sa_send->sa_family, SOCK_DGRAM, 0);
  
  // TODO
  proto->sa_bind->sa_family = proto->sa_send->sa_family;
  
  // our source UDP port
  s_port = (getpid() & 0xffff) | 0x8000; 
  sock_set_port(proto->sa_bind, proto->sa_len, htons(s_port));
  _bind(fd_send, proto->sa_bind, proto->sa_len);
  
  sig_alrm(SIGALRM);
  
  seq = 0;
  done = 0;
  for(ttl=1; (ttl<=max_ttl) && (done == 0); ++ttl){
    _setsockopt(fd_send, proto->ttl_level, proto->ttl_optname, &ttl, sizeof(int));
    bzero(proto->sa_last, proto->sa_len);
    
    printf("%2d ", ttl);
    fflush(stdout);
    
    for(probe=0; probe<n_probes; ++probe){
      rec = (struct rec*) sendbuf;
      rec->rec_seq = ++seq;
      rec->rec_ttl = ttl;
      _gettimeofday(&rec->rec_tv, NULL);
      
      sock_set_port(proto->sa_send, proto->sa_len, htons(d_port + seq));
      _sendto(fd_send, sendbuf, datalen, 0, proto->sa_send, proto->sa_len);
      
      if(-3 == (code = (*proto->recv) (seq, &tv_recv))){
        // timeout, no reply
        printf(" *");
        
      }else{
        char str[NI_MAXHOST];
        if(0 != sock_cmp_addr(proto->sa_recv, proto->sa_last, proto->sa_len)){
          if(0 == getnameinfo(proto->sa_recv, proto->sa_len, str, sizeof(str), NULL, 0, 0)){
            printf(" %s (%s)", str, _sock_ntop_host(proto->sa_recv, proto->sa_len));
          }else{
            printf(" %s", _sock_ntop_host(proto->sa_recv, proto->sa_len));
          }
          memcpy(proto->sa_last, proto->sa_recv, proto->sa_len);
        }
        tv_sub(&tv_recv, &rec->rec_tv);
        rtt = tv_recv.tv_sec * 1000.0 + tv_recv.tv_usec / 1000.0;
        printf(" %.3f ms", rtt);
        
        // prot unreachable, at destination
        if(code == -1){
          ++done;
        }else if(0 <= code){
          printf(" (ICMP %s)", (*proto->icmpcode) (code));
        }
      }
      fflush(stdout);
    }
    printf("\n");
  }
}


// TODO check
//extern int gotalarm;


/*
  receive function, returns
  -3 on timeout
  -2 on ICMP time exceeded in transit (caller keeps going)
  -1 on ICMP prot unreachable (caller is done)
  >= 0 return value is some other ICMP unreachable code
//*/
int recv_v4(int seq, struct timeval* tv)
{
  int hlen1, hlen2, icmplen, ret;
  socklen_t len;
  ssize_t cnt;
  struct ip *ip=NULL, *ip_host=NULL;
  struct icmp *icmp=NULL;
  struct udphdr *udp=NULL;

  gotalarm = 0;
  alarm(3);
  while(1){
    // alarm expired
    if(gotalarm){
      return -3;
    }

    len = proto->sa_len;
    cnt = recvfrom(fd_recv, recvbuf, sizeof(recvbuf), 0, proto->sa_recv, &len);
    if(cnt < 0){
      if(errno == EINTR){
        continue;
      }else{
        err_sys("recvfrom error");
      }
    }
    
    // start of reading IP header
    ip = (struct ip*) recvbuf;
    
    // length of IP header
    hlen1 = ip->ip_hl << 2;
    
    // start of ICMP header
    icmp = (struct icmp*) (recvbuf + hlen1);
    
    if(8 > (icmplen = cnt - hlen1)){
      // not enough to look at ICMP header
      continue;
    }
    
    if((icmp->icmp_type == ICMP_TIMXCEED) && (icmp->icmp_code == ICMP_TIMXCEED_INTRANS)){
      if(icmplen < 8 + sizeof(struct ip)){
        // not enough data to look at inner IP
        continue;
      }

      ip_host = (struct ip*) (recvbuf + hlen1 + 8);
      hlen2 = ip_host->ip_hl << 2;
      if(icmplen < 8 + hlen2 + 4){
        // not enough data to look at UDP ports
        continue;
      }

      udp = (struct udphdr*) (recvbuf + hlen1 + 8 + hlen2);
      if((ip_host->ip_p == IPPROTO_UDP) 
         && (udp->source == htons(s_port)) 
         && (udp->dest == htons(d_port)) ){
        
        // we hit an intermediate router
        ret = -2;
        break;
      }

    }else if(icmp->icmp_type == ICMP_UNREACH){
      if(icmplen < 8 + sizeof(struct ip)){
        // not enough data to look at inner IP
        continue;
      }

      ip_host = (struct ip*) (recvbuf + hlen1 + 8);
      hlen2 = ip_host->ip_hl << 2;
      if(icmplen < 8 + hlen2 + 4){
        // not enough data to look at UDP ports
        continue;
      }
      
      udp = (struct udphdr*) (recvbuf + hlen1 + 8 + hlen2);
      if((ip_host->ip_p == IPPROTO_UDP) 
         && (udp->source == htons(s_port)) 
         && (udp->dest = htons(d_port + seq))){

        if(icmp->icmp_code == ICMP_UNREACH_PORT){
          // have reached destination
          ret = -1;
        }else{
          // 0, 1, 2, ... 
          ret = icmp->icmp_code;
        }

        break;
      }
    }
    if(verbose){
      printf(" (from %s: type=%d, code %d)\n"
             , _sock_ntop_host(proto->sa_recv, proto->sa_len)
             , icmp->icmp_type
             , icmp->icmp_code);
    }
    // some other ICMP error, recvfrom() again
  }

  // don't leave alarm running
  alarm(0);

  // get time of packet arrival
  _gettimeofday(tv, NULL);
  
  return ret;
}


/*
  receive function, returns
  -3 on timeout
  -2 on ICMP time exceeded in transit (caller keeps going)
  -1 on ICMP prot unreachable (caller is done)
  >= 0 return value is some other ICMP unreachable code
//*/
int recv_v6(int seq, struct timeval *tv)
{     
#ifdef IPV6
  int hlen2, icmp6len, ret;
  ssize_t cnt;
  socklen_t len;
  struct ip6_hdr *ip_host6=NULL;
  struct icmp6_hdr *icmp6=NULL;
  struct udphdr *udp=NULL;
  
  gotalarm = 0;
  alarm(3);
  while(1){
    if(gotalarm){
      // alarm expired
      return -3;
    }

    len = proto->sa_len;
    if(0 > (cnt = recvfrom(fd_recv, recvbuf, sizeof(recvbuf), 0, proto->sa_recv, &len))){
      if(errno == EINTR){
        continue;
      }else{
        err_sys("recvfrom error");
      }
    }

    // ICMP header
    icmp6 = (struct icmp6_hdr*) recvbuf; 
    if(8 > (icmp6len = cnt)){
      // not enough to look at ICMP header
      continue;
    }

    if( (icmp6->icmp6_type == ICMP6_TIME_EXCEEDED) 
        && (icmp6->icmp6_code == ICMP6_TIME_EXCEED_TRANSIT)){
      
      if(icmp6len < (8 + sizeof(struct ip6_hdr) + 4)){
        // not enough data to look at inner header
        continue;
      }
      
      ip_host6 = (struct ip6_hdr*) (recvbuf + 8);
      hlen2 = sizeof(struct ip6_hdr);
      udp = (struct udphdr*) (recvbuf + 8);
      if((ip_host6->ip6_nxt == IPPROTO_UDP) 
         && (udp->source == htons(s_port)) 
         && (udp->dest == htons(d_port + seq)) ){

        // we hit an intermediate router
        ret = -2;
      }
      break;
      
    }else if(icmp6->icmp6_type == ICMP6_DST_UNREACH){
      if(icmp6len < 8 + sizeof(struct ip6_hdr) + 4){
        // not enough data to look at inner header
        continue;
      }
      
      ip_host6 = (struct ip6_hdr*) (recvbuf + 8);
      hlen2 = sizeof(struct ip6_hdr);
      udp = (struct udphdr*) (recvbuf + 8 + hlen2);
      if((ip_host6->ip6_nxt == IPPROTO_UDP) 
         && (udp->source == htons(s_port)) 
         && (udp->dest == htons(d_port + seq))){

        if(icmp6->icmp6_code == ICMP6_DST_UNREACH_NOPORT){
          ret = -1;
        }else{
          ret = icmp6->icmp6_code; // 0, 1, 2, ...
        }
        break;
      }
    }else if(verbose){
      printf(" (from %s: type=%d, code=%d)\n"
             , _sock_ntop_host(proto->sa_recv, proto->sa_len)
             , icmp6->icmp6_type
             , icmp6->icmp6_code);
    }
    // some other ICMP error, recvfrom() again
  }
  
  alarm(0);
  _gettimeofday(tv, NULL);
  return ret;
  
#endif
}


/*
  TODO
//*/
const char* icmpcode_v4(int code)
{
  static char errbuf[100];

  switch(code){
  case  0: 
    return("network unreachable");
  case  1:
    return("host unreachable");
  case  2:
    return("protocol unreachable");
  case  3:
    return("port unreachable");
  case  4:
    return("fragmentation required but DF bit set");
  case  5:
    return("source route failed");
  case  6:
    return("destination network unknown");
  case  7:
    return("destination host unknown");
  case  8:
    return("source host isolated (obsolete)");
  case  9:
    return("destination network administratively prohibited");
  case 10:
    return("destination host administratively prohibited");
  case 11:
    return("network unreachable for TOS");
  case 12:
    return("host unreachable for TOS");
  case 13:
    return("communication administratively prohibited by filtering");
  case 14:
    return("host recedence violation");
  case 15:
    return("precedence cutoff in effect");
 
  default:
    sprintf(errbuf, "[unknown code %d]", code);
    return errbuf;
  }
}


/*
  TODO
//*/
const char* icmpcode_v6(int code)
{
#ifdef IPV6
  static char errbuf[100];
  switch(code){
  case ICMP6_DST_UNREACH_NOROUTE:
    return "no route to host";    
  case ICMP6_DST_UNREACH_ADMIN:
    return "administratively prohibited";
  case ICMP6_DST_UNREACH_BEYONDSCOPE:
    return "beyond scope of source address";    
  case ICMP6_DST_UNREACH_ADDR:
    return "address unreachable";
  case ICMP6_DST_UNREACH_NOPORT:
    return "port unreachable";
  default:
    sprintf(errbuf, "[unknown code %d]", code);
    return errbuf;
  }
#endif
}


/*
  default inits 
//*/
struct protocol proto_v4 = { icmpcode_v4, recv_v4, NULL, NULL, NULL, NULL, 0, IPPROTO_ICMP, IPPROTO_IP, IP_TTL };
#ifdef IPV6
struct protocol proto_v6 = { icmpcode_v6, recv_v6, NULL, NULL, NULL, NULL, 0, IPPROTO_ICMPV6, IPPROTO_IPV6, IPV6_UNICAST_HOPS };
#endif

int datalen = sizeof(struct rec); // XXX redefinition, already defined in .h
int max_ttl = 30; // XXX redefinition, already defined in .h
int n_probes = 3; // XXX redefinition, already defined in .h
u_short d_port = 32768 + 666; // XXX redefinition, already defined in .h


/*
  main function
//*/
int main(int argc, char** argv)
{
  int cnt;
  struct addrinfo *ai=NULL;
  char *pchr=NULL;
  
  // don't want getopt() writing to stderr
  opterr = 0; 
  while(-1 != (cnt = getopt(argc, argv, "m:v"))){
    switch(cnt){
    case 'm':
      if(1 >= (max_ttl = atoi(optarg))){
        err_quit("invalid -m value");
      }
      break;

    case 'v':
      ++verbose;
      break;

    case '?':
      err_quit("unrecognized option: %c", cnt);
      break;
    }
  }

  if(optind != argc - 1){
    err_quit("usage: traceroute [ -m <maxttl> -v ] <hostname>");
  }

  host = argv[optind];

  pid = getpid();
  _signal(SIGALRM, sig_alrm);

  ai = _host_serv(host, NULL, 0, 0);
  pchr = _sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
  printf("traceroute to %s (%s): %d hops max, %d data bytes\n"
         , (ai->ai_canonname ? ai->ai_canonname : pchr)
         , pchr
         , max_ttl
         , datalen);

  // initialize according to protocol
  if(ai->ai_family == AF_INET){
    proto = &proto_v4;

#ifdef IPV4
  }else if(ai->ai_family == AF_INET6){
    proto = &proto_v6;
    if(IN6_IS_ADDR_V4MAPPED( &(((struct sockaddr_in6*) ai->ai_addr)->sin6_addr))){
      err_quit("cannot traceroute IPv4-mapped IPv6 address");
    }
#endif
  }else{
    err_quit("unknown address family %d", ai->ai_family);
  }

  // contains destination address
  proto->sa_send = ai->ai_addr;

  proto->sa_recv = _calloc(1, ai->ai_addrlen);
  proto->sa_last = _calloc(1, ai->ai_addrlen);
  proto->sa_bind = _calloc(1, ai->ai_addrlen);
  proto->sa_len = ai->ai_addrlen;

  traceloop();
  
  exit(0);
}

  
