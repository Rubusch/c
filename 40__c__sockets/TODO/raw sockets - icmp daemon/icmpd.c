// icmpd.c
/*
  icmp daemon that can handle ICMP4 and ICMP6 mechanism.

  the program works as daemon, receives and filters all packets
  entering the kernel, though works in user space.
//*/

#include "icmpd.h"

#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>

#ifdef IPV6
# include <netinet/ip6.h>
# include <netinet/icmp6.h>
#endif


/*
  TODO
//*/
int readable_v6()
{
#ifdef IPV6
  int idx, hlen2, icmp6len, s_port;
  char buf[MAXLINE];
  char str_src[INET6_ADDRSTRLEN], str_dst[INET6_ADDRSTRLEN];
  ssize_t cnt;
  socklen_t len;
  struct ip6_hdr *ip6_host=NULL;
  struct icmp6_hdr *icmp6=NULL;
  struct udphdr *udp=NULL;
  struct sockaddr_in6 src, dst;
  struct icmpd_err icmpd_err;

  len = sizeof(src);
  cnt = _recvfrom(fd_v6, buf, MAXLINE, 0, (SA*) &src, &len);
  
  printf("%d bytes ICMPv6 from %s:", cnt, _sock_ntop_host((SA*) &src, len));
  
  // start of ICMPv6 header
  icmp6 = (struct icmp6_hdr*) buf;
  if(8 > (icmp6len = cnt)){
    err_quit("icmp6len (%d) < 8", icmp6len);
  }
  
  printf(" type = %d, code = %d\n", icmp6->icmp6_type, icmp6->icmp6_code); 
  if((icmp6->icmp6_type == ICMP6_DST_UNREACH) 
     || (icmp6->icmp6_type == ICMP6_PACKET_TOO_BIG) 
     || (icmp6->icmp6_type == ICMP6_TIME_EXCEEDED)){
    
    if(icmp6len < 8 + 8){
      err_quit("icmp6len (%d) < 8 + 8", icmp6len);
    }
    
    ip6_host = (struct ip6_hdr*) (buf + 8);
    hlen2 = sizeof(struct ip6_hdr);
    printf("\tip_src = %s, ip_dst = %s, next hdr = %d\n"
           , _inet_ntop(AF_INET6, &ip6_host->ip6_src, str_src, sizeof(str_src))
           , _inet_ntop(AF_INET6, &ip6_host->ip6_dst, str_dst, sizeof(str_dst))
           , ip6_host->ip6_nxt);
    
    if(ip6_host->ip6_nxt == IPPROTO_UDP){
      udp = (struct udphdr*) (buf + 8 + hlen2);
      s_port = udp->source;
      
      // find client's unix domain socket, send headers
      for(idx=0; idx<=idx_max; ++idx){
        if((0 <= client[idx].fd_conn) 
           && (client[idx].family == AF_INET6)
           && (client[idx].l_port == s_port)){
          
          bzero(&dst, sizeof(dst));
          dst.sin6_family = AF_INET6;

#ifdef HAVE_SOCKADDR_SA_LEN
          dst.sin6_len = sizeof(dst);
#endif
          
          memcpy(&dst.sin6_addr, &ip6_host->ip6_dst, sizeof(struct in6_addr));
          dst.sin6_port = udp->dest;

          icmpd_err.icmpd_type = icmp6->icmp6_type;
          icmpd_err.icmpd_code = icmp6->icmp6_code;
          icmpd_err.icmpd_len = sizeof(struct sockaddr_in6);
          memcpy(&icmpd_err.icmpd_dest, &dst, sizeof(dst));
          
          // convert type & code to reasonable errno value
          icmpd_err.icmpd_errno = EHOSTUNREACH; // default
          if((icmp6->icmp6_type == ICMP6_DST_UNREACH) 
             && (icmp6->icmp6_code == ICMP6_DST_UNREACH_NOPORT)){
            
            icmpd_err.icmpd_errno = ECONNREFUSED;
          }
          
          if(icmp6->icmp6_type == ICMP6_PACKET_TOO_BIG){
            icmpd_err.icmpd_errno = EMSGSIZE;
          }

          _write(client[idx].fd_conn, &icmpd_err, sizeof(icmpd_err));
        }
      }
    }
  }

  return --n_ready;
#endif
}


/*
  TODO
//*/
int readable_v4()
{
  int idx, hlen1, hlen2, icmplen, s_port;
  char buf[MAXLINE], str_src[INET_ADDRSTRLEN], str_dst[INET_ADDRSTRLEN];
  ssize_t cnt;
  socklen_t len;
  struct ip *ip=NULL, *ip_host=NULL;
  struct icmp *icmp=NULL;
  struct udphdr *udp=NULL;
  struct sockaddr_in src, dst;
  struct icmpd_err icmpd_err;
  
  len = sizeof(src);
  cnt = _recvfrom(fd_v4, buf, MAXLINE, 0, (SA*) &src, &len);

  printf("%d bytes ICMPv4 from %s:", cnt, _sock_ntop_host((SA*) &src, len));

  // start of IP header
  ip = (struct ip*) buf;

  // length of IP header
  hlen1 = ip->ip_hl << 2;
  
  // start of ICMP header
  icmp = (struct icmp*) (buf + hlen1);

  if(8 > (icmplen = cnt - hlen1)){
    err_quit("icmplen (%d) < 8", icmplen);
  }

  printf(" type = %d, code = %d\n", icmp->icmp_type, icmp->icmp_code);
  if((icmp->icmp_type == ICMP_UNREACH) 
     || (icmp->icmp_type == ICMP_TIME_EXCEEDED) 
     || (icmp->icmp_type == ICMP_SOURCEQUENCH)){
    
    if(icmplen < 8 + 20 + 8){
      err_quit("icmplen (%d) < 8 + 20 + 8", icmplen);
    }
    
    ip_host = (struct ip*) (buf + hlen1 + 8);
    hlen2 = ip_host->ip_hl << 2;
    printf("\tip_src = %s, ip_dst = %s, proto = %d\n"
           , _inet_ntop(AF_INET, &ip_host->ip_src, str_src, sizeof(str_src))
           , _inet_ntop(AF_INET, &ip_host->ip_dst, str_dst, sizeof(str_dst))
           , ip_host->ip_p);
     
    if(ip_host->ip_p == IPPROTO_UDP){
      udp = (struct udphdr*) (buf + hlen1 + 8 + hlen2);
      s_port = udp->source;
      
      // find client's unix domain socket, send headers
      for(idx=0; idx<=idx_max; ++idx){
        if((0 <= client[idx].fd_conn)
           && (client[idx].family == AF_INET)
           && (client[idx].l_port == s_port)){
          
          bzero(&dst, sizeof(dst));
          dst.sin_family = AF_INET;

#ifdef HAVE_SOCKADDR_SA_LEN
          dst.sin_len = sizeof(dst);
#endif
          
          memcpy(&dst.sin_addr, &ip_host->ip_dst, sizeof(struct in_addr));
          dst.sin_port = udp->dest;
          
          icmpd_err.icmpd_type = icmp->icmp_type;
          icmpd_err.icmpd_code = icmp->icmp_code;
          icmpd_err.icmpd_len = sizeof(struct sockaddr_in);
          memcpy(&icmpd_err.icmpd_dest, &dst, sizeof(dst));
          
          // convert type & code to reasonable errno value
          icmpd_err.icmpd_errno = EHOSTUNREACH; // default
          if(icmp->icmp_type == ICMP_UNREACH){
            if(icmp->icmp_code == ICMP_UNREACH_PORT){
              icmpd_err.icmpd_errno = ECONNREFUSED;
            }else if(icmp->icmp_code == ICMP_UNREACH_NEEDFRAG){
              icmpd_err.icmpd_errno = EMSGSIZE;
            }
          }
          _write(client[idx].fd_conn, &icmpd_err, sizeof(icmpd_err));
        }
      }
    }
  }
  return --n_ready;
}


/*
  TODO
//*/
int readable_listen()
{
  int32_t idx, fd_conn;
  socklen_t clilen;
  
  clilen = sizeof(cliaddr);
  fd_conn = _accept(fd_listen, (SA*) &cliaddr, &clilen);
  
  // find first available client[] structure
  for(idx=0; idx<FD_SETSIZE; ++idx){
    if(0 > client[idx].fd_conn){
      // save descriptor
      client[idx].fd_conn = fd_conn;
      break;
    }
  }

  if(idx == FD_SETSIZE){
    // can't handle new client
    // rudely close the new connection
    close(fd_conn);
    return --n_ready;
  }
  printf("new connection, idx = %d, fd_conn = %d\n", idx, fd_conn);

  // add new descriptor to set (for select)  
  FD_SET(fd_conn, &allset);
  if(fd_conn > fd_max){
    fd_max = fd_conn;
  }

  // max index in client[] array
  if(idx > idx_max){
    idx_max = idx;
  }

  return --n_ready;    
}


/*
  TODO
//*/
int readable_conn(int idx)
{
  int fd_unix, fd_recv;
  char chr;
  ssize_t cnt;
  socklen_t len;
  struct sockaddr_storage ss;
  
  fd_unix = client[idx].fd_conn;
  fd_recv = -1;
  if(0 == (cnt = _read_fd(fd_unix, &chr, 1, &fd_recv))){
    err_msg("client %d terminated, fd_recv = %d", idx, fd_recv);
    goto clientdone; // HANDLE IMMEDIATELY! client probably terminated
  }

  // data from client; should be descriptor
  if(0 > fd_recv){
    err_msg("fd_read did not return descriptor");
    goto clienterr; // HANDLE IMMEDIATELY!
  }

  len = sizeof(ss);
  if(0 > getsockname(fd_recv, (SA*) &ss, &len)){
    err_ret("getsockname error");
    goto clienterr; // HANDLE IMMEDIATELY!
  }

  client[idx].family = ss.ss_family;
  if(0 == (client[idx].l_port = sock_get_port((SA*) &ss, len))){
    client[idx].l_port = sock_bind_wild(fd_recv, client[idx].family);
    if(0 >= client[idx].l_port){
      err_ret("error binding ephemeral port");
      goto clienterr; // HANDLE IMMEDIATELY!
    }
  }

  // tell client all OK
  _write(fd_unix, "1", 1);
 
  // all done with client's UDP socket
  _close(fd_recv);
  
  return --n_ready;
  
  // LABELS
 clienterr:
  _write(fd_unix, "0", 1);
  
 clientdone:
  _close(fd_unix);
  if(0 >= fd_recv){
    _close(fd_recv);
  }
  FD_CLR(fd_unix, &allset);
  client[idx].fd_conn = -1;

  return --n_ready;
}


/*
  main...
//*/
int main(int argc, char** argv)
{
  int32_t idx, fd_sock;
  
  struct sockaddr_un sun;
  if(argc != 1){
    err_quit("usage: icmpd");
  }

  // index into client[] array
  idx_max = -1;

  for(idx=0; idx < FD_SETSIZE; ++idx){
    // -1 indicates available entry
    client[idx].fd_conn = -1;
  }
  
  FD_ZERO(&allset);  

  // ICMPv4 raw socket
  fd_v4 = _socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  FD_SET(fd_v4, &allset);  
  fd_max = fd_v4;

#ifdef IPV6
  // ICMPv6 raw socket
  fd_v6 = _socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
  FD_SET(fd_v6, &allset);
  fd_max = fd_v6;
#endif

  // UNIX socket
  fd_listen = _socket(AF_UNIX, SOCK_STREAM, 0);
  sun.sun_family = AF_LOCAL;
  strcpy(sun.sun_path, ICMPD_PATH);
  unlink(ICMPD_PATH);
  _bind(fd_listen, (SA*) &sun, sizeof(sun));
  _listen(fd_listen, LISTENQ);

  // TODO
  FD_SET(fd_listen, &allset);

  // TODO
  fd_max = max(fd_max, fd_listen);

  while(1){
    // TODO
    rset = allset;

    // TODO
    n_ready = _select(fd_max +1, &rset, NULL, NULL, NULL);
    
    if(FD_ISSET(fd_listen, &rset)){
      if(0 >= readable_listen()){
        continue;
      }
    }
    
    if(FD_ISSET(fd_v4, &rset)){
      if(0 >= readable_v4()){
        continue;
      }
    }

#ifdef IPV6
    if(FD_ISSET(fd_v6, &rset)){
      if(0 >= readable_v6()){
        continue;
      }
    }
#endif

    // check all clients for data
    for(idx=0; idx<=idx_max; ++idx){
      if(0 > (fd_sock = client[idx].fd_conn)){
        continue;
      }
      
      if(FD_ISSET(fd_sock, &rset)){
        if(0 >= readable_conn(idx)){
          // no more readable description
          break;
        }
      }
    } // for
  } // while(1)

  exit(0);
}

