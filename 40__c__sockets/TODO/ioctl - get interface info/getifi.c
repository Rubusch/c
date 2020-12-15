// getifi.c
/*
  get interface info (472)
//*/

#include "../lib_socket/lib_ifi/ifi.h"

int main(int argc, char** argv)
{
  struct ifi_info *ifi, *ifihead;
  struct sockaddr *sa;
  u_char *ptr;
  int idx, family, doaliases;
  
  // set family to IPv4 or IPv6
  family = AF_INET;
#ifdef AF_INET6
  //family = AF_INET6;
#endif

  // set to 1 (true) or 0 (false)
  doaliases = 1;

  for(ifihead = ifi = _get_ifi_info(family, doaliases)
        ; ifi != NULL
        ; ifi = ifi->ifi_next){
    printf("%s: ", ifi->ifi_name);
    if(ifi->ifi_index != 0){
      printf("(%d) ", ifi->ifi_index);
    }

    printf("< ");
    if(ifi->ifi_flags & IFF_UP) printf("UP ");
    if(ifi->ifi_flags & IFF_BROADCAST) printf("BCAST ");
    if(ifi->ifi_flags & IFF_MULTICAST) printf("MCAST ");
    if(ifi->ifi_flags & IFF_LOOPBACK) printf("LOOP ");
    if(ifi->ifi_flags & IFF_POINTOPOINT) printf("P2P ");
    printf(">\n");
    
    if(0 < (idx = ifi->ifi_hlen)){
      ptr = ifi->ifi_haddr;
      do{
        printf("%s%x", (idx == ifi->ifi_hlen) ? " " : ":", *ptr++);
      }while(--idx > 0);      
    }
    
    if(0 != ifi->ifi_mtu){
      printf(" MTU: %d\n", ifi->ifi_mtu);
    }

    if(NULL != (sa = ifi->ifi_addr)){
      printf(" IP addr: %s\n", _sock_ntop_host(sa, sizeof(*sa)));
    }

    if(NULL != (sa = ifi->ifi_brdaddr)){
      printf(" broadcast addr: %s\n", _sock_ntop_host(sa, sizeof(*sa)));
    }

    if(NULL != (sa = ifi->ifi_dstaddr)){
      printf(" destination addr: %s\n", _sock_ntop_host(sa, sizeof(*sa)));
    }
  }
  free_ifi_info(ifihead);
  exit(0);
}

