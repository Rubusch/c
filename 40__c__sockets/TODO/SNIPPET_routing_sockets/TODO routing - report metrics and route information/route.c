// route.c
/*
  routing sockets

  report metrics and other routing information via RTM_GET 
  (to and from kernel)
//*/

#include "../lib_socket/lib_route/route.h"


// sizeof(struct sockaddr_in6) * 8 = 192
#define BUFLEN (sizeof(struct rt_msghdr) + 512)
#define SEQ 9999

int main(int argc, char** argv)
{
  int fd_sock;
  char *buf=NULL;
  pid_t pid;
  ssize_t num;
  struct rt_msghdr *rtm=NULL;
  struct sockaddr *sa, *rti_info[RTAX_MAX];
  struct sockaddr_in *sin;

  // need superuser privileges (routing socket)
  fd_sock = _socket(AF_ROUTE, SOCK_RAW, 0); 

  // and initialized to 0
  buf = _calloc(1, BUFLEN); 

  rtm = (struct rt_msghdr*) buf;
  rtm->rtm_msglen = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);
  rtm->rtm_version = RTM_VERSION;
  rtm->rtm_type = RTM_GET;
  rtm->rtm_addrs = RTA_DST;
  rtm->rtm_pid = pid = getpid();
  rtm->rtm_seq = SEQ;
  
  sin = (struct sockaddr_in*) (rtm + 1);
  sin->sin_len = sizeof(struct sockaddr_in);
  sin->sin_family = AF_INET;

  // set socket on ip 10.0.2.2
  _inet_pton(AF_INET, "10.0.2.2", &sin->sin_addr);
  
  // write
  _write(fd_sock, rtm, rtm->rtm_msglen);

  // read
  do{
    num = _read(fd_sock, rtm, BUFLEN);
  }while(rtm->rtm_type != RTM_GET || rtm->rtm_seq != SEQ || rtm->rtm_pid != pid);

  // output
  rtm = (struct rt_msghdr*) buf;
  sa = (struct sockaddr*) (rtm + 1);
  get_rtaddrs(rtm->rtm_addrs, sa, rti_info);

  if(NULL != (sa = rti_info[RTAX_DST])){
    printf("dest: %s\n", _sock_ntop_host(sa, sa->sa_len));
  }

  if(NULL != (sa = rti_info[RTAX_GATEWAY])){
    printf("gateway: %s\n", _sock_ntop_host(sa, sa->sa_len));
  }

  if(NULL != (sa = rti_info[RTAX_NETMASK])){
    printf("netmask: %s\n", _sock_masktop(sa, sa->sa_len));
  }
  
  if(NULL != (sa = rti_info[RTAX_GENMASK])){
    printf("genmask: %s\n", _sock_masktop(sa, sa->sa_len));
  }

  exit(0);
}

