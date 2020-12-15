// client.c
/*
  SCTP one-to-many style streaming echo client

  TODO p. 300: graceful shutdown 
//*/

#include "../lib_socket/lib_socket.h"
//#include <netinet/sctp.h> // TODO check

sctpstr_cli(FILE *fp, int fd_sock, struct sockaddr* to, socklen_t tolen)
{
  struct sockaddr_in peeraddr;
  struct sctp_sndrcvinfo sri;
  char sendline[MAXLINE], recvline[MAXLINE];
  socklen_t len;
  int sz_out, sz_in;
  int msg_flags;

  bzero(&sri, sizeof(sri));
  while(fgets(sendline, MAXLINE, fp) != NULL){
    if(sendline[0] != '['){
      printf("error, line must be of the form '[streamnum]text'\n");
      continue;
    }
    sri.sinfo_stream = strtol(&sendline[1], NULL, 0);
    sz_out = strlen(sendline);
    _sctp_sendmsg(fd_sock, senline, sz_out, to, tolen, 0, 0, sri.sinfo_stream, 0, 0);
    
    len = sizeof(peeraddr);
    sz_in = _sctp_recvmsg(fd_sock, recvline, sizeof(recvline), (SA*) &peeraddr, &len, &sri, &msg_flags);
    printf("from str: %d seq: %d (assoc: 0x%x):", sri.sinfo_stream, sri.sinfo_ssn, (u_int) sri.sinfo_assoc_id);
    printf("%.*s", sz_rd, recvline);
  }
}


#define SCTP_MAXLINE 800

void sctpstr_cli_echoall(FILE* fp, int fd_sock, struct sockaddr* to, socklen_t tolen)
{
  struct sockaddr_in peeraddr;
  struct sctp_sndrcvinfo sri;
  char sendline[SCTP_MAXLINE], recvline[SCTP_MAXLINE];
  socklen_t len;
  int sz_in, idx, sz_str, msg_flags;

  bzero(sendline, sizeof(sendline));
  bzero(&sri, sizeof(sri));
  while(NULL != fgets(sendline, SCTP_MAXLINE - 9, fp)){
    sz_str = strlen(sendline);
    if(sendline[sz_str - 1] == '\n'){
      sendline[sz_str - 1] = '\0';
      --sz_str;
    }

    /* modification 1
    for(idx=0; idx<SERV_MAX_SCTP_STRM; ++idx){
      snprintf(sendline + sz_str, sizeof(sendline) - sz_str, ".msg.%d 1", idx);
      _sctp_sendmsg(fd_sock, sendline, sizeof(sendline), to, tolen, 0, 0, idx, 0, 0);
      snprintf(sendline + sz_str, sizeof(sendline) - sz_str, ".msg.%d 2", idx);
      _sctp_sendmsg(fd_sock, sendline, sizeof(sendline), to, tolen, 0, 0, idx, 0, 0);
    }

    for(idx=0; idx<SERV_MAX_SCTP_STRM * 2; ++idx){
      len = sizeof(peeraddr);
    }
    //*/

    for(idx=0; idx<SERV_MAX_SCTP_STRM; ++idx){
      snprintf(sendline + sz_str, sizeof(sendline) - sz_str, ".msg.%d", idx);
      _sctp_sendmsg(fd_sock, sendline, sizeof(sendline), to, tolen, 0, 0, idx, 0, 0);
    }

    for(idx=0; idx<SERV_MAX_SCTP_STRM; ++idx){
      len = sizeof(peeraddr);
      sz_in = _sctp_recvmsg(fd_sock, recvline, sizeof(recvline), (SA*) &peeraddr, &len, &sri, &msg_flags);
      printf("from str:%d seq:%d (assoc:0x%x):", sri.sinfo_stream, sri.sinfo_ssn, (u_int) sri.sinfo_assoc_id);
      printf("%.*s\n", sz_in, recvline);
    }
  }
}


int main(int argc, char** argv)
{
  int fd_sock;
  struct sockaddr_in servaddr;
  struct sctp_event_subscribe events;
  int echo_to_all = 0;

  // TODO check 
  echo_to_all = 1;
  
  fd_sock = _socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  _inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  
  bzero(&events, sizeof(events));
  events.sctp_data_io_event = 1;
  _setsockopt(fd_sock, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));
  if(echo_to_all == 0){
    sctpstr_cli(stdin, fd_sock, (SA*) &servaddr, sizeof(servaddr));
  }else{
    sctpstr_cli_echoall(stdin, fd_sock, (SA*) &servaddr, sizeof(servaddr));
  }

  _close(fd_sock);
  return 0;
}
