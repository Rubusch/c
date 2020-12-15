// mcast.c
/*
  contains
  - mcast_get_if.c
  - mcast_get_loop.c
  - mcast_get_ttl.c
  - mcast_join.c
  - mcast_leave.c
  - mcast_set_if
  - mcast_set_loop.c
  - mcast_set_ttl.c
//*/

#include "lib_socket.h"

#include <net/if.h>


int mcast_get_if(int sockfd)
{
  switch (sockfd_to_family(sockfd)){
  case AF_INET: 
    {
      // TODO: similar to mcast_set_if()
      return -1;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      u_int idx;
      socklen_t len;
      len = sizeof(idx);
      
      if(getsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &idx, &len) < 0){
        return -1;
      }
      return idx ;
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
}



int _mcast_get_if(int sockfd)
{
  int rc;  
  if(0 > (rc = mcast_get_if(sockfd))){
    err_sys("mcast_get_if error");
  }
  return rc;
}



int mcast_get_loop(int sockfd)
{
  switch (sockfd_to_family(sockfd)){
  case AF_INET: 
    {
      u_char  flag;
      socklen_t len;
      
      len = sizeof(flag);
      if(0 > getsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &flag, &len)){
        return -1;
      }
      return flag;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      u_int  flag;
      socklen_t len;
      len = sizeof(flag);
      if(0 > getsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &flag, &len)){
        return -1;
      }
      return flag;
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
}



int _mcast_get_loop(int sockfd)
{
  int rc;  
  if(0 > (rc = mcast_get_loop(sockfd))){
    err_sys("mcast_get_loop error");
  }
  return rc;
}



int mcast_get_ttl(int sockfd)
{
  switch (sockfd_to_family(sockfd)){
  case AF_INET: 
    {
      u_char  ttl;
      socklen_t len;
      
      len = sizeof(ttl);
      if(0 > getsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, &len)){
        return -1;
      }
      return ttl;
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      int hop;
      socklen_t len;
      len = sizeof(hop);
      if(0 > getsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hop, &len)){
        return(-1);
      }
      return hop;
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return(-1);
  }
}



int _mcast_get_ttl(int sockfd)
{
  int rc;  
  if(0 > (rc = mcast_get_ttl(sockfd))){
    err_sys("mcast_get_ttl error");
  }
  return rc;
}



int mcast_join( int sockfd
                , const SA *grp
                , socklen_t grplen
                , const char *ifname
                , u_int ifindex)
{
#ifdef MCAST_JOIN_GROUP
  struct group_req req;
  if(0 < ifindex){
    req.gr_interface = ifindex;
  }else if(NULL != ifname){
    if(0 == (req.gr_interface = if_nametoindex(ifname))){
      errno = ENXIO; // interface name not found 
      return -1;
    }
  }else{
    req.gr_interface = 0;
  }

  if(grplen > sizeof(req.gr_group)){
    errno = EINVAL;
    return -1;
  }

  memcpy(&req.gr_group, grp, grplen);
  return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_JOIN_GROUP, &req, sizeof(req));
#else
  switch(grp->sa_family){
  case AF_INET: 
    {
      struct ip_mreq  mreq;
      struct ifreq  ifreq;
      
      memcpy(&mreq.imr_multiaddr, &((const struct sockaddr_in *) grp)->sin_addr, sizeof(struct in_addr));
      
      if(0 < ifindex){
        if(NULL == if_indextoname(ifindex, ifreq.ifr_name)){
          errno = ENXIO; // i/f index not found
          return -1;
        }
        goto doioctl;

      }else if(NULL != ifname){
        strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
      doioctl:
        if(0 > ioctl(sockfd, SIOCGIFADDR, &ifreq)){
          return -1;
        }

        memcpy(&mreq.imr_interface, &((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr, sizeof(struct in_addr));
      }else{
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      }
      
      return setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    }
#ifdef IPV6
#ifndef IPV6_JOIN_GROUP  // APIv0 compatibility
#define IPV6_JOIN_GROUP  IPV6_ADD_MEMBERSHIP
#endif
  case AF_INET6: 
    {
      struct ipv6_mreq mreq6;
      
      memcpy(&mreq6.ipv6mr_multiaddr, &((const struct sockaddr_in6 *) grp)->sin6_addr, sizeof(struct in6_addr));
      
      if(0 < ifindex){
        mreq6.ipv6mr_interface = ifindex;
      }else if(NULL != ifname){
        if(0 == (mreq6.ipv6mr_interface = if_nametoindex(ifname))){
          errno = ENXIO; // i/f name not found
          return -1;
        }
      }else{
        mreq6.ipv6mr_interface = 0;
      }
      
      return setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6));
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
#endif
}



void _mcast_join( int sockfd
                  , const SA *grp
                  , socklen_t grplen
                  , const char *ifname
                  , u_int ifindex)
{
  if(0 > mcast_join(sockfd, grp, grplen, ifname, ifindex)){
    err_sys("mcast_join error");
  }
}



int mcast_join_source_group( int sockfd
                             , const SA *src
                             , socklen_t srclen
                             , const SA *grp
                             , socklen_t grplen
                             , const char *ifname
                             , u_int ifindex)
{
#ifdef MCAST_JOIN_SOURCE_GROUP
  struct group_source_req req;
  if(0 < ifindex){
    req.gsr_interface = ifindex;
  }else if(NULL != ifname){
    if(0 == (req.gsr_interface = if_nametoindex(ifname))){
      errno = ENXIO; /* i/f name not found */
      return -1;
    }
  }else{
    req.gsr_interface = 0;
  }

  if((grplen > sizeof(req.gsr_group)) || (srclen > sizeof(req.gsr_source))){
    errno = EINVAL;
    return -1;
  }
  memcpy(&req.gsr_group, grp, grplen);
  memcpy(&req.gsr_source, src, srclen);
  return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_JOIN_SOURCE_GROUP, &req, sizeof(req));
#else
  switch(grp->sa_family) {
#ifdef IP_ADD_SOURCE_MEMBERSHIP
  case AF_INET: 
    {
      struct ip_mreq_source mreq;
      struct ifreq   ifreq;
      
      memcpy(&mreq.imr_multiaddr, &((struct sockaddr_in *) grp)->sin_addr, sizeof(struct in_addr));
      memcpy(&mreq.imr_sourceaddr, &((struct sockaddr_in *) src)->sin_addr, sizeof(struct in_addr));
      
      if(ifindex > 0) {
        if (if_indextoname(ifindex, ifreq.ifr_name) == NULL) {
          errno = ENXIO; /* i/f index not found */
          return(-1);
        }
        goto doioctl;
      } else if (ifname != NULL) {
        strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
      doioctl:
        if (ioctl(sockfd, SIOCGIFADDR, &ifreq) < 0)
          return(-1);
        memcpy(&mreq.imr_interface, &((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr, sizeof(struct in_addr));
      } else
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      
      return(setsockopt(sockfd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, &mreq, sizeof(mreq)));
    }
#endif
    
#ifdef IPV6
  case AF_INET6: // IPv6 source-specific API is MCAST_JOIN_SOURCE_GROUP 
#endif
  default:
    errno = EAFNOSUPPORT;
    return(-1);
  }
#endif
}



void _mcast_join_source_group( int sockfd
                               , const SA *src
                               , socklen_t srclen
                               , const SA *grp
                               , socklen_t grplen
                               , const char *ifname
                               , u_int ifindex)
{
  if(0 > mcast_join_source_group(sockfd, src, srclen, grp, grplen, ifname, ifindex)){
    err_sys("mcast_join_source_group error");
  }
}



int mcast_block_source( int sockfd
                        , const SA *src
                        , socklen_t srclen
                        , const SA *grp
                        , socklen_t grplen)
{
#ifdef MCAST_BLOCK_SOURCE
  struct group_source_req req;
  req.gsr_interface = 0;
  if( (grplen > sizeof(req.gsr_group)) || (srclen > sizeof(req.gsr_source))){
    errno = EINVAL;
    return -1;
  }
  memcpy(&req.gsr_group, grp, grplen);
  memcpy(&req.gsr_source, src, srclen);
  return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_BLOCK_SOURCE, &req, sizeof(req));
#else
  switch(grp->sa_family){
#ifdef IP_BLOCK_SOURCE
  case AF_INET: 
    {
      struct ip_mreq_source mreq;
      
      memcpy(&mreq.imr_multiaddr, &((struct sockaddr_in *) grp)->sin_addr, sizeof(struct in_addr));
      memcpy(&mreq.imr_sourceaddr, &((struct sockaddr_in *) src)->sin_addr, sizeof(struct in_addr));
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      
      return setsockopt(sockfd, IPPROTO_IP, IP_BLOCK_SOURCE, &mreq, sizeof(mreq));
    }
#endif
    
#ifdef IPV6
  case AF_INET6: // IPv6 source-specific API is MCAST_BLOCK_SOURCE
#endif
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
#endif
}

void _mcast_block_source( int sockfd
                          , const SA *src
                          , socklen_t srclen
                          , const SA *grp
                          , socklen_t grplen)
{
  if(0 > mcast_block_source(sockfd, src, srclen, grp, grplen)){
    err_sys("mcast_block_source error");
  }
}



int mcast_unblock_source( int sockfd
                          , const SA *src
                          , socklen_t srclen
                          , const SA *grp
                          , socklen_t grplen)
{
#ifdef MCAST_UNBLOCK_SOURCE
  struct group_source_req req;
  req.gsr_interface = 0;
  if( (grplen > sizeof(req.gsr_group)) || (srclen > sizeof(req.gsr_source))){
    errno = EINVAL;
    return -1;
  }
  memcpy(&req.gsr_group, grp, grplen);
  memcpy(&req.gsr_source, src, srclen);
  return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_UNBLOCK_SOURCE, &req, sizeof(req));
#else
  switch(grp->sa_family){
#ifdef IP_UNBLOCK_SOURCE
  case AF_INET: 
    {
      struct ip_mreq_source mreq;
      
      memcpy(&mreq.imr_multiaddr, &((struct sockaddr_in *) grp)->sin_addr, sizeof(struct in_addr));
      memcpy(&mreq.imr_sourceaddr, &((struct sockaddr_in *) src)->sin_addr, sizeof(struct in_addr));
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      
      return setsockopt(sockfd, IPPROTO_IP, IP_UNBLOCK_SOURCE, &mreq, sizeof(mreq));
    }
#endif
    
#ifdef IPV6
  case AF_INET6: // IPv6 source-specific API is MCAST_UNBLOCK_SOURCE
#endif
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
#endif
}



void _mcast_unblock_source( int sockfd
                            , const SA *src
                            , socklen_t srclen
                            , const SA *grp
                            , socklen_t grplen)
{
  if(0 > mcast_unblock_source(sockfd, src, srclen, grp, grplen)){
    err_sys("mcast_unblock_source error");
  }
}



int mcast_leave(int sockfd, const SA *grp, socklen_t grplen)
{
#ifdef MCAST_JOIN_GROUP
  struct group_req req;
  req.gr_interface = 0;
  if(grplen > sizeof(req.gr_group)){
    errno = EINVAL;
    return -1;
  }
  memcpy(&req.gr_group, grp, grplen);
  return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_LEAVE_GROUP, &req, sizeof(req));
#else
  switch(grp->sa_family){
  case AF_INET: 
    {
      struct ip_mreq  mreq;
      
      memcpy(&mreq.imr_multiaddr, &((const struct sockaddr_in *) grp)->sin_addr, sizeof(struct in_addr));
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      return setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    }
    
#ifdef IPV6
#ifndef IPV6_LEAVE_GROUP // APIv0 compatibility
#define IPV6_LEAVE_GROUP IPV6_DROP_MEMBERSHIP
#endif

  case AF_INET6: 
    {
      struct ipv6_mreq mreq6;
      
      memcpy(&mreq6.ipv6mr_multiaddr, &((const struct sockaddr_in6 *) grp)->sin6_addr, sizeof(struct in6_addr));
      mreq6.ipv6mr_interface = 0;
      return setsockopt(sockfd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &mreq6, sizeof(mreq6));
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
#endif
}



void _mcast_leave( int sockfd
                   , const SA *grp
                   , socklen_t grplen)
{
  if(0 > mcast_leave(sockfd, grp, grplen)){
    err_sys("mcast_leave error");
  }
}



int mcast_leave_source_group( int sockfd
                              , const SA *src
                              , socklen_t srclen
                              , const SA *grp
                              , socklen_t grplen)
{
#ifdef MCAST_LEAVE_SOURCE_GROUP
  struct group_source_req req;
  req.gsr_interface = 0;
  if( (grplen > sizeof(req.gsr_group)) || (srclen > sizeof(req.gsr_source))){
    errno = EINVAL;
    return -1;
  }
  memcpy(&req.gsr_group, grp, grplen);
  memcpy(&req.gsr_source, src, srclen);
  return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_LEAVE_SOURCE_GROUP, &req, sizeof(req));
#else
  switch(grp->sa_family){
#ifdef IP_DROP_SOURCE_MEMBERSHIP
  case AF_INET: 
    {
      struct ip_mreq_source mreq;
      
      memcpy(&mreq.imr_multiaddr, &((struct sockaddr_in *) grp)->sin_addr, sizeof(struct in_addr));
      memcpy(&mreq.imr_sourceaddr, &((struct sockaddr_in *) src)->sin_addr, sizeof(struct in_addr));
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      
      return setsockopt(sockfd, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, &mreq, sizeof(mreq));
    }
#endif
    
#ifdef IPV6
  case AF_INET6: // IPv6 source-specific API is MCAST_LEAVE_SOURCE_GROUP
#endif

  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
#endif
}



void _mcast_leave_source_group( int sockfd
                                , const SA *src
                                , socklen_t srclen
                                , const SA *grp
                                , socklen_t grplen)
{
  if(0 > mcast_leave_source_group(sockfd, src, srclen, grp, grplen)){
    err_sys("mcast_leave_source_group error");
  }
}



int mcast_set_if( int sockfd
                  , const char *ifname
                  , u_int ifindex)
{
  switch(sockfd_to_family(sockfd)){
  case AF_INET: 
    {
      struct in_addr  inaddr;
      struct ifreq  ifreq;
      
      if(0 < ifindex){
        if(NULL == if_indextoname(ifindex, ifreq.ifr_name)){
          errno = ENXIO; // i/f index not found
          return -1;
        }
        goto doioctl;
        
      }else if(NULL != ifname){
        strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
      doioctl:
        if(0 > ioctl(sockfd, SIOCGIFADDR, &ifreq)){
          return -1;
        }
        memcpy(&inaddr, &((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr, sizeof(struct in_addr));
      }else{
        inaddr.s_addr = htonl(INADDR_ANY); // remove prev. set default 
      }

      return setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &inaddr, sizeof(struct in_addr));
  }
  
#ifdef IPV6
  case AF_INET6: 
    {
      u_int idx;
      
      if(0 == (idx = ifindex)){
        if(NULL == ifname){
          errno = EINVAL; // must supply either index or name
          return -1;
        }
        
        if(0 == (idx = if_nametoindex(ifname))){
          errno = ENXIO; // i/f name not found
          return -1;
        }
      }
      return setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &idx, sizeof(idx));
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
}



void _mcast_set_if( int sockfd
                    , const char *ifname
                    , u_int ifindex)
{
  if(0 > mcast_set_if(sockfd, ifname, ifindex)){
    err_sys("mcast_set_if error");
  }
}



int mcast_set_loop( int sockfd
                    , int onoff)
{
  switch(sockfd_to_family(sockfd)){
  case AF_INET: 
    {
      u_char flag;
      
      flag = onoff;
      return setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &flag, sizeof(flag));
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      u_int flag;
      
      flag = onoff;
      return setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &flag, sizeof(flag));
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
}



void _mcast_set_loop( int sockfd
                      , int onoff)
{
  if(0 > mcast_set_loop(sockfd, onoff)){
    err_sys("mcast_set_loop error");
  }
}



int mcast_set_ttl( int sockfd
                   , int val)
{
  switch(sockfd_to_family(sockfd)){
  case AF_INET: 
    {
      u_char ttl;
      
      ttl = val;
      return setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    }
    
#ifdef IPV6
  case AF_INET6: 
    {
      int hop;
      
      hop = val;
      return setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hop, sizeof(hop));
    }
#endif
    
  default:
    errno = EAFNOSUPPORT;
    return -1;
  }
}



void _mcast_set_ttl( int sockfd
                     , int val)
{
  if(0 > mcast_set_ttl(sockfd, val)){
    err_sys("mcast_set_ttl error");
  }
}
