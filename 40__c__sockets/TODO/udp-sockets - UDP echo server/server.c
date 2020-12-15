// server.c
/*
  UDP echo server using select()

  demonstrates usage of select
//*/

#include "../lib_socket/lib_socket.h"

/*
  mini signal handler, calls waitpid to avoid zombies
//*/
void sig_child(int signo)
{
  pid_t pid;
  int status;

  while(0 < (pid = waitpid(-1, &status, WNOHANG))){
    printf("child %d terminated\n", pid);
  }
}


/*
  udp echo server implementation
//*/
int main(int argc, char** argv)
{
  int fd_listen, fd_conn, fd_udp, n_ready, maxfdpl;
  char msg[MAXLINE];
  pid_t childpid;
  fd_set rset;
  ssize_t num;
  socklen_t len;
  const int on = 1;
  struct sockaddr_in cliaddr, servaddr;

  // create listening TCP socket
  fd_listen = _socket(AF_INET, SOCK_STREAM, 0);
   
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  _setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  
  _bind(fd_listen, (SA*) &servaddr, LISTENQ);
  
  // create UDP socket
  fd_udp = _socket(AF_INET, SOCK_DGRAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  
  _bind(fd_udp, (SA*) &servaddr, sizeof(servaddr));

  _signal(SIGCHLD, sig_child); // must call waitpid()

  // 1. zero fd_set - select()
  FD_ZERO(&rset); 

  maxfdpl = max(fd_listen, fd_udp) + 1;
  while(1){
    
    // 2. add descriptors to the fd_set - select()
    FD_SET( fd_listen, &rset);
    FD_SET( fd_udp, &rset);

    // 3. select() multiplexing
    if(0 > (n_ready = select(maxfdpl, &rset, NULL, NULL, NULL))){
      if(errno == EINTR){ // caught interrupt
        continue;
      }else{
        err_sys("select error");
      }
    }
    
    // 4. select case "fd_listen"
    if(FD_ISSET(fd_listen, &rset)){
      len = sizeof(cliaddr);
      fd_conn = _accept(fd_listen, (SA*) &cliaddr, &len);
      
      if(0 == (childpid = _fork())){ 
        // child process
        _close(fd_listen);
        str_echo(fd_conn);
        exit(0);
      }

      // parent process
      _close(fd_conn);
    }

    // 4. select case "fd_udp"
    if(FD_ISSET(fd_udp, &rset)){
      len = sizeof(cliaddr);
      num = _recvfrom(fd_udp, msg, MAXLINE, 0, (SA*) &cliaddr, &len);
      
      _sendto(fd_udp, msg, num, 0, (SA*) &cliaddr, len);
    }
  }  
}

