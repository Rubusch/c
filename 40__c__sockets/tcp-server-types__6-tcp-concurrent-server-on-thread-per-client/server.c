// server.c
/*
  TCP concurrent server, one thread per client

  fastest server, handling several threads / processes
//*/



/*
  constants
*/

// TODO


/*
  forwards
*/

// TODO


/*
  internal helpers
*/

// TODO


/*
  helpers / wrappers

  mainly to cover error handling and display error message
*/

// TODO


/*
  main

  TODO
 */


// client address pointer
static struct sockaddr* cliaddr;


/*
  print server and user time
//*/
void pr_cpu_time()
{
  double user, sys;
  struct rusage usage_parent, usage_child;
  if(0 > getrusage(RUSAGE_SELF, &usage_parent)){
    err_sys("getrusage(parent) error");
  }
  
  if(0 > getrusage(RUSAGE_CHILDREN, &usage_child)){
    err_sys("getrusage(child) error");
  }
  
  user = (double) usage_parent.ru_utime.tv_sec + usage_parent.ru_utime.tv_usec / 1000000.0;
  user += (double) usage_child.ru_utime.tv_sec + usage_child.ru_utime.tv_usec / 1000000.0;

  sys = (double) usage_parent.ru_stime.tv_sec + usage_parent.ru_stime.tv_usec / 1000000.0;
  sys += (double) usage_child.ru_stime.tv_sec + usage_child.ru_stime.tv_usec / 1000000.0;
  
  printf("\nuser time = %g, sys time = %g\n", user, sys);
}


/*
  signal handler on SIGTERM, shutting down the server
//*/
void sig_int(int32_t signo)
{
  pr_cpu_time();

  if(NULL != cliaddr){
    free(cliaddr);
    cliaddr = NULL;
  }

  exit(0);
}


// max line size
#define MAXN 1234

/*
  example code: do anything, read, write, etc.. some action
//*/
void child_routine(int32_t fd_sock)
{
  int32_t towrite;
  ssize_t n_read;
  char line[MAXLINE], result[MAXN];

  while(1){
    // read
    if(0 == (n_read = _readline(fd_sock, line, MAXLINE))){
      return; // connection closed by other end
    }

    // get as long
    towrite = atol(line);

    // check if valid
    if((0 >= towrite) || (MAXN < towrite)){
      err_quit("client request for %d bytes", towrite);
    }
    
    // if ok, write to socket
    _write(fd_sock, result, towrite);
  }
}


/*
  thread handler function
//*/
void* thread_handler(void *arg)
{
  // detach thread, to run and finish separately 
  _pthread_detach(pthread_self());

  // do something, never returns..
  child_routine((int32_t) arg);

  // close socket
  _close((int32_t) arg);

  return NULL;
}


/*
  main...
//*/
int32_t main(int32_t argc, char** argv)
{
  int32_t fd_listen, fd_conn;
  
  pthread_t tid;
  socklen_t clilen, addrlen;

  // listen on any ip, port 27976
  fd_listen = _tcp_listen(NULL, "27976", &addrlen);
  // ...or alternatively on ip 10.0.2.15, port 27976
  //fd_listen = _tcp_listen("10.0.2.15", "27976", &addrlen);

  cliaddr = _malloc(addrlen);
  
  _signal(SIGINT, sig_int);

  while(1){ // loops
    clilen = addrlen;

    // 1. accept client
    fd_conn = _accept(fd_listen, cliaddr, &clilen);

    // 2. create thread for client
    _pthread_create(&tid, NULL, &thread_handler, (void*) fd_conn);
  }  
  
  return 0;
}
