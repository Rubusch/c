// inetd server
/*
  SECURITY PROBLEM - unsave server (avoid internet connection)
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT 1976


void sighandler(int dummy)
{
  int stat;
  pid_t pid = wait(&stat);
  printf("Child %i exited.\n", pid);
}


void start_app(int hSocket)
{
  close(0); // close stdin
  close(1); // close stdout
  close(2); // close stderr

  /*
    dup allows to use hSocket as handle equally to the streams
    also possible: dup2() - TODO
  //*/
  dup(hSocket); // hSocket becomes 0 -> stdin
  dup(hSocket); // hSocket becomes 1 -> stdout
  dup(hSocket); // hSocket becomes 2 -> stderr

  // close Socket
  close(hSocket);
  
  // new process group
  setsid();

  // execl won't return, if successfull
  execl("/bin/sh", "sh", NULL);
  
  perror("execl() failed");
  exit(1);
}


int main(void)
{
  int hSocket=0, hClient=0;
  struct sockaddr_in addrClient;
  pid_t pid;
  
  if(0 > (hSocket = socket(PF_INET, SOCK_STREAM, 0))){
    perror("socket failed");
    return EXIT_FAILURE;
  }

  // bind
  addrClient.sin_addr.s_addr = INADDR_ANY;
  addrClient.sin_family = PF_INET;
  addrClient.sin_port = htons(PORT);
  if(0 > bind(hSocket, (struct sockaddr*) &addrClient, sizeof(addrClient))){
    perror("bind failed");
    return EXIT_FAILURE;
  }

  // listen
  if(0 > listen(hSocket, 3)){
    perror("listen failed");
    return EXIT_FAILURE;
  }


  /*
    process won't be detached as daemon, but stays on the console
  //*/

  //* signal - SIG_ERR -> SIGCHLD
  if(signal(SIGCHLD, sighandler) == SIG_ERR){
    perror("singal failed, ho ho ho");
    return EXIT_FAILURE;
  }
  //*/
  /* unix: sigaction - unknown
  struct sigaction sigAct;
  memset(&sigAct, 0, sizeof(sigAct));
  sigAct.sa_handler = sighandler;
  sigaction(SIGCHLD, &sigAct, NULL);
  //*/ 

  unsigned int run_server = 1;
  while(run_server){
    if(0 > (hClient = accept(hSocket, NULL, 0))){
      if(errno != EINTR){
	perror("accept failed - client dead");
      }
      continue;
    }

    switch(pid = fork())
      {
      case -1:
	perror("fork failed");
	break;
      case 0:
	start_app(hClient);
	break;
      default:
	printf("child: %i\n", pid);
	break;
      }
  }

  // close missing?!
  exit(EXIT_SUCCESS);
}
  
