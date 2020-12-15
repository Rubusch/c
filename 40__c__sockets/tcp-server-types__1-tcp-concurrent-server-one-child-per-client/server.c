// server.c
/*
  TCP concurrent server, one child per client

  slowest TCP server type
//*/

#include "../lib_socket/lib_socket.h"


/********************************************************************************************/
// example code - code for some demo actions, not necessarily related to the server type

// max character number to write
#define MAXN 1234 

/*
  example code: do anything, read, write, etc.. some action
//*/
void child_routine(int fd_sock)
{
  int32_t towrite;
  ssize_t n_read;
  char line[MAXLINE], result[MAXN];

  // server loop
  while(1){
    // read
    if(0 == (n_read = _readline(fd_sock, line, MAXLINE))){
      // nothing more to read -> exit
      fprintf(stdout, "reading over, %d returns\n", getpid());
      return; 
    }

    // get as long's
    towrite = atol(line);

    // check if valid
    if((0 >= towrite) || (MAXN < towrite)){
      err_quit("client request for %d bytes", towrite);
    }
    
    // if ok, write and return the received message back to socket
    _write(fd_sock, result, towrite);
  }
}


/*
  child signal handler - calls waitpid()

  called when a child process has terminated the work
//*/
void sig_chld(int signo){
  pid_t pid;
  int32_t status;
  
  while(0 < (pid = waitpid(-1, &status, WNOHANG))){
    printf("child %d terminated\n", pid);
  }
}


/*
  example code: print system time and usage at CTRL + C termination of server
//*/
void pr_cpu_time()
{
  double user, sys;
  struct rusage usage_parent, usage_child;

  // init parent ressource usage
  if(0 > getrusage(RUSAGE_SELF, &usage_parent)){
    err_sys("getrusage(parent) error");
  }

  // init child ressource usage
  if(0 > getrusage(RUSAGE_CHILDREN, &usage_child)){
    err_sys("getrusage(child) error");
  }

  // calculate user time
  user = (double) usage_parent.ru_utime.tv_sec + usage_parent.ru_utime.tv_usec / 1000000.0;
  user += (double) usage_child.ru_utime.tv_sec + usage_child.ru_utime.tv_usec / 1000000.0;

  // calculate system time
  sys = (double) usage_parent.ru_stime.tv_sec + usage_parent.ru_stime.tv_usec / 1000000.0;
  sys += (double) usage_child.ru_stime.tv_sec + usage_child.ru_stime.tv_usec / 1000000.0;

  // print message at termination of server
  printf("\nuser time = %gs, sys time = %gs\n", user, sys);
}


/*
  final sig handler - action when server is shutdown by CTRL + c
//*/
void sig_int(int signo)
{
  pr_cpu_time();
  exit(0);
}

/********************************************************************************************/

/*
  main - server implementation
//*/
int main(int argc, char** argv)
{
  int32_t fd_listen, fd_conn;
  pid_t child_pid;
  socklen_t clilen, addrlen;
  struct sockaddr *cliaddr=NULL;
  cliaddr = _malloc(addrlen);

  // set socket listen on port 27976
  fd_listen = _tcp_listen(NULL, "27976", &addrlen);
  // or alternatively, listen on host ip 10.0.2.15 and port 27976
  //fd_listen = _tcp_listen("10.0.2.15", "27976", &addrlen);

  // set signal handlers
  _signal(SIGCHLD, sig_chld);
  _signal(SIGINT, sig_int);

  while(1){
    clilen = addrlen;
    if(0 > (fd_conn = accept(fd_listen, cliaddr, &clilen))){
      if(errno == EINTR){
        continue;
      }
      // or
      err_sys("accept() error");
    }

    if(0 == (child_pid = _fork())){
      // child process

      // close listening socket
      _close(fd_listen);

      // process loop - never returnes
      child_routine(fd_conn);

      exit(0);
    }

    // parent process..

    // closes connected socket
    _close(fd_conn); 
    if(NULL != cliaddr) free(cliaddr); cliaddr = NULL;
  }  

  return 0;
}

