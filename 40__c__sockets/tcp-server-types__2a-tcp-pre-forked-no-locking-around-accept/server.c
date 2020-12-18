// server.c
/*
  TCP preforked server, no locking around accept

  (each child calling accept)
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


static int32_t n_children;
static pid_t *pid_children;


/*
  example code: print system times when shutting down by CTRL + c
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
  
  printf("\nuser time = %gs, sys time = %gs\n", user, sys);
}


/*
  final action when server is shutdown by CTRL + c
//*/
void sig_int(int32_t signo)
{
  int32_t idx;
  
  for(idx=0; idx<n_children; ++idx){
    kill(pid_children[idx], SIGTERM);
  }
  free(pid_children); pid_children = NULL;

  while(0 < wait(NULL))
    ;
  
  if(errno != ECHILD){
    err_sys("wait() error");
  }

  pr_cpu_time();
  exit(0);
}


// max character number to be written
#define MAXN 1234

/*
  child sub, called by child main (do some read, and echo write)
//*/
void child_routine(int32_t fd_sock)
{
  int32_t towrite;
  ssize_t n_read;
  char line[MAXLINE], result[MAXN];

  while(1){
    // read
    if(0 == (n_read = _readline(fd_sock, line, MAXLINE))){
      // end of reading, exit
      return; 
    }

    // get as long
    towrite = atol(line);
    
    // check if valid, echo
    if((0 >= towrite) || (MAXN < towrite)){
      err_quit("client request for %d bytes", towrite);
    }
    
    _write(fd_sock, result, towrite);
  }
}


/*
  child main
//*/
void child_main(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
  int32_t fd_conn;
  socklen_t clilen;

  struct sockaddr cliaddr[addrlen];

  printf("child %ld starting\n", (long) getpid());

  while(1){ 

    /*
      WARNING: accept can cause "collisions", this here is only 
      the template for the following "preforked" implementations, 
      therefore here no locking at all - blocking by accept!

      In a real scenario there should be at least a select() before 
      accept()
    //*/

    clilen = addrlen;

    // blocks...
    fd_conn = _accept(fd_listen, cliaddr, &clilen);

    child_routine(fd_conn);

    _close(fd_conn);
  }
}


/*
  fork
//*/
pid_t child_make(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
  pid_t pid;

  if(0 == (pid = _fork())){
    // child
    child_main(idx, fd_listen, addrlen); 
    // never returns
  }

  // parent  
  return pid; 
}


/*
  main...
//*/
int32_t main(int32_t argc, char** argv)
{
  int32_t fd_listen, idx;
  socklen_t addrlen;

  // listen on any ip, on port 27976
  fd_listen = _tcp_listen(NULL, "27976", &addrlen);
  // ..or alternatively, listen on ip 10.0.2.15, on port 17976
  //fd_listen = _tcp_listen("10.0.2.15", "27976", &addrlen);

  // set number of children
  n_children = 7;
  pid_children = _calloc(n_children, sizeof(*pid_children));
  for(idx=0; idx<n_children; ++idx){
    // create children, only parent returns
    pid_children[idx] = child_make(idx, fd_listen, addrlen); 
  }

  // signal handler, set up only once in parent for children
  _signal(SIGINT, sig_int);

  while(1){
    pause();
  }  

  return 0;
}
