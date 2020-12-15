// hostmac.c
/*
  print host's hardware address (483)
//*/

#include "../lib_socket/lib_ifi/ifi.h"
#include <net/if_arp.h>


/*
  main... 
//*/
int main(int argc, char** argv)
{
  int fd_sock;
  struct ifi_info *ifi;
  u_char *ptr;
  struct arpreq arpreq;
  struct sockaddr_in *sin;
  
  fd_sock = _socket(AF_INET, SOCK_DGRAM, 0);
  for(ifi = get_ifi_info(AF_INET, 0)
        ; ifi != NULL
        ; ifi = ifi->ifi_next){
    printf("%s: ", _sock_ntop(ifi->ifi_addr, sizeof(struct sockaddr_in)));
    
    sin = (struct sockaddr_in*) &arpreq.arp_pa;
    memcpy(sin, ifi->ifi_addr, sizeof(struct sockaddr_in));
    
    if(0 > ioctl(fd_sock, SIOCGARP, &arpreq)){
      err_ret("ioctl SIOCGARP");
      continue;
    }

    ptr = &arpreq.arp_ha.sa_data[0];
    printf("%x:%x:%x:%x:%x:%x\n", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
  }

  exit(0);  
}

