// server.c
/*
  TCP preforked server, file locking around accept

  (with file lock to protect accept)
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

// the file locks
static struct flock lock_it, unlock_it;
static int32_t lock_fd = -1;


/*
  fcntl() will fail if lock_init() not called! 
//*/
void lock_init(char* pathname)
{
  char lock_file[1024];
  
  // must copy caller's string, in case it's a constant
  if((1024 - 7) <= strlen(pathname)){
    exit(1);
  }

  // mkstemp() - tmp file MUST end with "XXXXXX"  
  strncpy(lock_file, pathname, sizeof(lock_file));
  strncat(lock_file, "XXXXXX", 1024); 

  //  lock_file[strlen(lock_file+1)] = '\0';
  lock_fd = _mkstemp(lock_file);
  
  // but lock_fd remains open
  _unlink(lock_file);

  lock_it.l_type = F_WRLCK;
  lock_it.l_whence = SEEK_SET;
  lock_it.l_start = 0;
  lock_it.l_len = 0;

  unlock_it.l_type = F_UNLCK;
  unlock_it.l_whence = SEEK_SET;
  unlock_it.l_start = 0;
  unlock_it.l_len = 0;
}


/*
  try file lock
//*/
void lock_wait()
{
  int32_t rc;
  while(0 > (rc = fcntl(lock_fd, F_SETLKW, &lock_it))){
    if(errno == EINTR){
      continue;
    }
    
    // or
    err_sys("fcntl error for lock_wait");
  }
}


/*
  release the file lock
//*/
void lock_release()
{
  if(0 > fcntl(lock_fd, F_SETLKW, &unlock_it)){
    err_sys("fcntl error for lock_release");
  }
}


/*
  print the user and system time on shutting down the server
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


// max character number to write
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
  child function
//*/
void child_main(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
  int32_t fd_conn;
  socklen_t clilen;
  struct sockaddr cliaddr[addrlen];

  printf("child %ld starting\n", (long) getpid());

  while(1){

    clilen = addrlen;

    // file lock, to avoid collisions on the blocking accept
    lock_wait();

    fd_conn = _accept(fd_listen, cliaddr, &clilen);

    // file unlock
    lock_release();

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
    child_main(idx, fd_listen, addrlen); // never returns
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
  // ..or alternatively, listen on ip 10.0.2.15, on port 27976
  //fd_listen = _tcp_listen("10.0.2.15", "27976", &addrlen);

  // set number of children
  n_children = 7;
  pid_children = _calloc(n_children, sizeof(*pid_children));

  // init file lock
  lock_init("/tmp/lock.1234567"); // one lock file for all children

  for(idx=0; idx<n_children; ++idx){
    pid_children[idx] = child_make(idx, fd_listen, addrlen); // only parent returns
  }
  
  _signal(SIGINT, sig_int);

  while(1){
    pause();
  }  

  // everything done by children
  return 0;
}
