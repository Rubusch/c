// sapsdp.c
/*
  multicast example to receive SAP / SDP anouncements

  SAP - session announcement protocol
  SDP - 
//*/

#include "../lib_socket/lib_socket.h"

// changeable settins - default group name and port
#define SAP_NAME          "sap.mcast.net" 
#define SAP_PORT          "9875"

// SAP / SDR defaults
#define SAP_VERSION       1
#define SAP_VERSION_MASK  0xe0000000
#define SAP_VERSION_SHIFT 29
#define SAP_IPV6          0x10000000
#define SAP_DELETE        0x04000000
#define SAP_ENCRYPTED     0x02000000
#define SAP_COMPRESSED    0x01000000
#define SAP_AUTHLEN_MASK  0x00ff0000
#define SAP_AUTHLEN_SHIFT 16
#define SAP_HASH_MASK     0x0000ffff


/*
  loop helper, do operation and print to console
//*/
void loop(int fd_sock, socklen_t salen)
{
  socklen_t len;
  ssize_t num;
  char *ptr;
  struct sockaddr *sa;
  struct sap_packet{
    uint32_t sap_header;
    uint32_t sap_src;
    char sap_data[BUFSIZ];
  } buf;

  sa = _malloc(salen);
  
  while(1){
    len = salen;
    num = _recvfrom(fd_sock, &buf, sizeof(buf) - 1, 0, sa, &len);

    // '\0' terminated
    ((char*) &buf)[num] = '\0';

    buf.sap_header = ntohl(buf.sap_header);
    
    printf("from %s hash 0x%04x\n", _sock_ntop(sa, len), buf.sap_header & SAP_HASH_MASK);
    if(1 < ((buf.sap_header & SAP_VERSION_MASK) >> SAP_VERSION_SHIFT)){
      err_msg("... version field not 1 (0x%08x)", buf.sap_header);
      continue;
    }

    if(buf.sap_header & SAP_IPV6){
      err_msg("... IPv6");
      continue;
    }

    if(buf.sap_header & (SAP_DELETE | SAP_ENCRYPTED | SAP_COMPRESSED)){
      err_msg("... can't parse this packet type (0x%08x)", buf.sap_header);
      continue;
    }

    ptr = buf.sap_data + ((buf.sap_header & SAP_AUTHLEN_MASK) >> SAP_AUTHLEN_SHIFT);
    
    if(0 == strcmp(ptr, "application / sdp")){
      ptr += 16;
    }

    printf("%s\n", ptr);
  }
}


/*
  main
//*/
int main(int argc, char** argv)
{
  int fd_sock;
  const int on = 1;
  socklen_t salen;
  struct sockaddr *sa;

  // set up socket and address to the specified name and port
  fd_sock = _udp_client(SAP_NAME, SAP_PORT, (void**) &sa, &salen);

  // set SOL_SOCKET (socket level manipulateable) and address reuseable
  _setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  // bind socket to address
  _bind(fd_sock, sa, salen);

  // multicast - join (alternatively change the "NULL" by the interface name)
  _mcast_join(fd_sock, sa, salen, NULL, 0);
  
  // receive and print
  loop(fd_sock, salen); 

  exit(0);
}
    
