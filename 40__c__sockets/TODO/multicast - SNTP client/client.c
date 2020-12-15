// client.c
/*
  SNTP - Simple Network Time Protocol 

  NTP client that listens for NTP broadcasts or multicasts 
  on all attached networks and then prints the time difference 
  between the NTP packet and the host's current time-of-day. 
  We do not try to adjust the time-of-day, as that takes 
  superuser privileges.
//*/

#include "../lib_socket/lib_ifi/ifi.h"
#include "sntp.h"

/*
  SNTP procedure
//*/
void sntp_proc(char *buf, ssize_t num, struct timeval *nowptr)
{
  int version, mode;
  uint32_t nsec, useci;
  double usecf;
  struct timeval diff;
  struct ntpdata *ntp=NULL;
  
  if(num < (ssize_t) sizeof(struct ntpdata)){
    printf("\npacket too small: %d bytes\n", num);
    return;
  }

  ntp = (struct ntpdata*) buf;
  version = (ntp->status & VERSION_MASK) >> 3;

  mode = ntp->status & MODE_MASK;
  printf("\nv%d, mode %d, strat %d, ", version, mode, ntp->stratum);
  if(mode == MODE_CLIENT){
    printf("client\n");
    return;
  }

  // date conversion
  nsec = ntohl(ntp->xmt.int_part) - JAN_1970;

  // 32-bit integer fraction
  useci = ntohl(ntp->xmt.fraction);
  
  // integer fraction -> double
  usecf = useci;

  // divide by 2^32 -> [0.0; 1.0]
  usecf /= 4294967296.0;

  // fraction -> parts per million
  useci = usecf * 1000000.0;

  if(0 > (diff.tv_sec = nowptr->tv_sec - nsec)){
    diff.tv_usec += 1000000;
    --diff.tv_sec;
  }

  // diff in microsec (network delay not taken into account!)
  useci = (diff.tv_sec * 1000000) + diff.tv_usec;
  printf("clock difference = %d usec\n", useci);
}


/*
  main..
//*/
int main(int argc, char** argv)
{
  int fd_sock;
  char buf[MAXLINE];
  ssize_t num;
  socklen_t salen, len;
  struct ifi_info *ifi=NULL;
  struct sockaddr *mcastsa=NULL, *wild=NULL, *from=NULL;
  struct timeval now;
  
  fd_sock = _udp_client("10.0.2.2", "ntp", (void*) &mcastsa, &salen);

  wild = _malloc(salen);

  // copy family and port
  memcpy(wild, mcastsa, salen);

  sock_set_wild(wild, salen);
  _bind(fd_sock, wild, salen);

#ifdef MCAST
  // obtain interface list and process each one
  for(ifi=_get_ifi_info(mcastsa->sa_family, 1)
        ; ifi != NULL
        ; ifi = ifi->ifi_next){
    if(ifi->ifi_flags & IFF_MULTICAST){
      _mcast_join(fd_sock, mcastsa, salen, ifi->ifi_name, 0);
      printf("joined %s on %s\n", _sock_ntop(mcastsa, salen), ifi->ifi_name);
    }
  }
#endif
  
  from = _malloc(salen);
  while(1){
    len = salen;
    num = _recvfrom(fd_sock, buf, sizeof(buf), 0, from, &len);
    _gettimeofday(&now, NULL);
    sntp_proc(buf, num, &now);
  }
}

  
