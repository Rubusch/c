// tcp_server.c
/*
  1. socket()
  
  (sockopts)
  
  2. bind()

  3. listen() - only server

  4. accept() - only server

  5. write()    | read()
     send()     | recv()
     sendto()   | recvfrom()
     writev()   | readv()
     sendmsg()  | recvmsg()

  6. close()


  - server forks connected clients out
  - various clients connect to the server which broadcasts the messages 
      to all connected clients
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <wait.h>

#define BUF_SIZ 1024
#define QUIT_SERVER "quit server"

// TODO: find solution to have a list of clients (list of hClients??)
// TODO: func to add to the list
// TODO: func to remove from the list
// TODO: iterator for the list
// TODO: func to get the length of the list
// TODO: func to send to clients in the list

// signal handler
void sighandler(int dymmy)
{
  wait(NULL);
}


// client fork code
void handle_client(const int hClient, unsigned int* run_server )
{
  char buffer[BUF_SIZ];
  memset(buffer, '\0', BUF_SIZ);
  int bytes=0;
  while(0 > (bytes = recv(hClient, buffer, sizeof(buffer), 0))){
    // XXX - rm: debugging output
    puts(buffer); 

    // quit server - TODO: test
    if(strncmp(buffer, QUIT_SERVER, strlen(QUIT_SERVER)) && (strlen(QUIT_SERVER) == strlen(buffer))){
      *run_server = 0;
      close(hClient);
    }

    // TODO:
    // broadcast message to other connected clients, but the one who sent

  }
  close(hClient);
}


int main(int argc, char** argv)
{
  // init
  int hSocket=0, hClient=0;
  socklen_t addrlen=0;
  struct sockaddr_in addrServer;
  memset(&addrServer, 0, sizeof(addrServer));
  if(2 != argc){
    puts("tcp_server <port>");
    exit(1);
  }

  // socket
  if(-1 == (hSocket = socket(PF_INET, SOCK_STREAM, 0))){
    perror("socket() failed");
    exit(1);
  }

  // sockopts
  /*
    socket option SO_REUSEADDR: allow bind() even if old 
    protocol instances are still using the address
  //*/
  int optval = 1;
  if(0 != setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))){
    perror("setsockopt"); 
    exit(1);
  }

  // bind
  addrServer.sin_port = htons(atoi(argv[1]));
  addrServer.sin_family = AF_INET;
  addrServer.sin_addr.s_addr = INADDR_ANY;
  if(-1 == (bind(hSocket, (struct sockaddr*) &addrServer, sizeof(addrServer)))){
    perror("bind() failed");
    exit(1);
  }

  // listen - SOMAXCONN, max number of conn's
  if(-1 == (listen(hSocket, 3))){
    perror("listen() failed");
    exit(1);
  }

  // signalhandler
  if(SIG_ERR == signal(SIGCHLD, sighandler)){
    perror("signal");
    exit(1);
  }

  unsigned int run_server = 1;
  while(run_server){
    // accept
    addrlen = sizeof(addrServer);
    if(-1 == (accept(hSocket, (struct sockaddr*) &addrServer, &addrlen))){
      perror("accept() failed");
      continue;
    }
    pid_t pid=0;
    if(0 > (pid = fork())){
      perror("fork() failed");
      exit(1);

    }else if(pid == 0){
      // child code
      handle_client(hClient, &run_server);
      exit(0);

    }else{
      // parent code

      close(hClient);
    }
  }
  close(hSocket);
  exit(0);
}
