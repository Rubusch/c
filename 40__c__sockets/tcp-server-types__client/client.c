// client.c
/*
  tcp server types - TCP testing client for all server types 
//*/

#include "../lib_socket/lib_socket.h"


// max number of bytes to request from server
#define MAXN 1234


int main(int argc, char** argv)
{
  int idx
    , jdx
    , fd_connect
    , n_children
    , n_loops
    , n_bytes;

  pid_t pid;
  ssize_t bytes;
  char request[MAXLINE], reply[MAXN];

  // number of children
  n_children = 7;

  // number of sockets to open, write, read and close again
  n_loops = 20;

  // number of max bytes to read
  n_bytes = 64;

  // init request to send to server - ends with \n
  snprintf(request, sizeof(request), "%d\n", n_bytes); 

  // per child...
  for(idx=0; idx<n_children; ++idx){

    // if is not parent!
    if(0 == (pid = _fork())){

      // per loop...
      for(jdx=0; jdx<n_loops; ++jdx){

        // connect to 10.0.2.2 via port 27976
        fd_connect = _tcp_connect("10.0.2.2", "27976");

        // write to socket
        _write(fd_connect, request, strlen(request));

        // read response
        if(n_bytes != (bytes = _readn(fd_connect, reply, n_bytes))){
          err_quit("server returned %d bytes", bytes);
        }
        
        // close socket
        _close(fd_connect);
      }
      printf("child %d done\n", idx);
      exit(0);
    }

  } // parent loops around to fork() again

  while(0 < wait(NULL))
    ;

  if(errno != ECHILD){
    err_sys("wait error");
  }

  exit(0);
}

