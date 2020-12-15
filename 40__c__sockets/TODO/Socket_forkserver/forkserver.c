// forkserver.c
/*
  <signal.h> doesn't contain a sigaction struct, nor a sigaction function
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define PORT 27976 //1976


int handle_client(int hSocket)
{
  char *message = "Hello, World!\r\n";
  send(hSocket, message, strlen(message), 0);
  return 0;
}


void sigchild_handler(int dummy)
{
  int status=0;
  pid_t pid = wait(&status);

  printf("%i exited with %i\n", pid, WEXITSTATUS(status));
}


void quit(char* text)
{
  perror(text);
  exit(1);
}


int main(int argc, char** argv)
{
  int hSocket=0, hClient;
  pid_t pid;
  struct sockaddr_in addrServer;
  
  if(-1 == (hSocket = socket(PF_INET, SOCK_STREAM, 0))) quit("socket() failed");

  addrServer.sin_addr.s_addr = INADDR_ANY;
  addrServer.sin_port = htons(PORT);
  addrServer.sin_family = AF_INET;

  if( -1 == (bind(hSocket, (struct sockaddr*) &addrServer, sizeof(addrServer)))) 
    quit("bind() failed");

  if(0 > listen(hSocket, 3)) quit("listen() failed"); 

  // signal SIGCHLD
  if(SIG_ERR == signal(SIGCHLD, sigchild_handler)){
    perror("signal failed");
    exit(1);
  }
  // sigaction approach doesn't work!!!
  /*
  struct sigaction sigAct; // TODO -> change usage in Makefile to "standard C99"
  memset(sigAct, 0, sizeof(sigAct));
  sigAct.sa_handler = sigchild_handler;
  sigaction(SIGCHLD, &sigAct, NULL);
  //*/

  while(1){
    if(0 > (hClient = accept(hSocket, NULL, 0))){
      if(errno != EINTR)
	perror("accept() failed");
      continue;
    }
    
    if(0 == (pid = fork()))
       exit(handle_client(hClient));
    
    printf("Client has PID %i\r\n", pid);
    close(hClient);
  }
  return EXIT_SUCCESS;
}
