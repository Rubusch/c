// server.c
/*
  SCTP one-to-many style streaming echo server
//*/

#include "../lib_socket/lib_socket.h"


int main(int argc, char** argv)
{
  int fd_sock, msg_flags;
  char readbuf[BUFSIZ];
  struct sockaddr_in servaddr, cliaddr;
  struct sctp_sndrcvinfo sri;
  struct sctp_event_subscribe events;
  int stream_increment = 1;
  socklen_t len;
  size_t sz_in;
  
  /* 
     When a message arrives, the server checks the stream_increment 
     flag to see if it should increment the stream number. If the 
     flag is set (no arguments where passed on the conmmand line), 
     the server increments the stream number of the message. If that
     number grows larger than or equal to the maximum streams, which 
     is obtained by calling our internal function call 
     sctp_get_no_strms is not sholwn. It uses the SCTP_STATUS SCTP 
     socket option.
  //*/
  stream_increment = 1; // TODO check

  /* modification 2
  fd_sock = _socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  bzero(&initm, sizeof(initm));
  initm.sinit_num_ostreams = SERV_MORE_STRMS_SCTP;
  _setsockopt(fd_sock, IPPROTO_SCTP, SCTP_INITMSG, &initm, sizeof(initm));
  //*/

  fd_sock = _socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  
  _bind(fd_sock, (SA*) &servaddr, sizeof(servaddr));

  bzero(&events, sizeof(events));
  
  // notifications
  events.sctp_data_io_event = 1;
  
  _setsockopt(fd_sock, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));
  
  _listen(fd_sock, LISTENQ);
  
  while(1){
    len = sizeof(struct sockaddr_in);
    sz_in = _sctp_recvmsg(fd_sock, readbuf, sizeof(readbuf), (SA*) &cliaddr, &len, &sri, &msg_flags);

    if(stream_increment){
      ++sri.sinfo_stream;

      if(sri.sinfo_stream >= sctp_get_no_strms(fd_sock, (SA*) &cliaddr, len)){
        sri.sinfo_stream = 0;
      }
    }
    _sctp_sendmsg(fd_sock, readbuf, sz_in, (SA*) &cliaddr, len, sri.sinfo_ppid, sri.sinfo_flags, sri.sinfo_stream, 0, 0);
  }
}
