// server.c
/*
  TCP preforked server, descriptor passing

  In the previous preforked examples, the process never cared 
  which child received a client connection. The OS handled this 
  detail, giving one of the children the first call to accept(),
  or giving one of the children the file lock or the mutex lock.
  The results also show that the OS that we are measuring does 
  this in a fair, round-robin fashion.

  FIXME!
//*/

#include "../lib_socket/lib_socket.h"

// number of children
static int32_t n_children = 7;

typedef struct{
  pid_t child_pid;   // process ID
  int32_t fd_child_pipe; // parent's stream pipe to / from child
  int32_t child_status;  // 0 = ready
  long child_count;  // number of connections handled
} Child_t;
Child_t* child_ptr;


/*
  print system and user time
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
  signal handler for SIGTERM - shutting down the server
//*/
void sig_int(int32_t signo)
{
  pr_cpu_time();
  if(NULL != child_ptr){
    free(child_ptr); 
    child_ptr = NULL;
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
  int32_t n_towrite;
  ssize_t n_read;
  char line[MAXLINE], result[MAXN];

  while(1){
    // read
    if(0 == (n_read = _readline(fd_sock, line, MAXLINE))){
      return; // connection closed by other end
    }

    // convert
    n_towrite = atol(line);

    // check
    if((0 >= n_towrite) || (MAXN < n_towrite)){
      err_quit("client request for %d bytes", n_towrite);
    }
    
    // if ok, write echo to socket
    _write(fd_sock, result, n_towrite);
  }
}


/*
  child main function
//*/
void child_main(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
  char chr;
  int32_t fd_conn;
  ssize_t num;

  printf("child %ld starting\n", (long) getpid());

  while(1){
    if(0 == (num = _read_fd(STDERR_FILENO, &chr, 1, &fd_conn))){
      err_quit("_read_fd returned 0");
    }
    
    if(0 > fd_conn){
      err_quit("no descriptor from _read_fd");
    }

    // process request
    child_routine(fd_conn);
    
    _close(fd_conn);

    // tell parent we're ready again
    _write(STDERR_FILENO, "", 1);
  }
}


/*
  fork function
//*/
pid_t child_make(int32_t idx, int32_t fd_listen, int32_t addrlen)
{
  int32_t fd_sock[2];
  pid_t pid;

  _socketpair(AF_LOCAL, SOCK_STREAM, 0, fd_sock);

  if(0 < (pid = _fork())){
    // close other end of pipe
    _close(fd_sock[1]); 

    // init Child_t struct to be passed via the pipe
    child_ptr[idx].child_pid = pid;
    child_ptr[idx].fd_child_pipe = fd_sock[0];
    child_ptr[idx].child_status = 0;

    // parent returns
    return pid; 
  }

  // child's stream pipe to parent
  _dup2(fd_sock[idx], STDERR_FILENO); 

  // close pipe
  _close(fd_sock[0]);
  _close(fd_sock[1]);

  // child does not need this open
  _close(fd_listen);
  
  // never returns!!
  child_main(idx, fd_listen, addrlen);

  return pid;
}


/*
  main..
//*/
int32_t main(int32_t argc, char** argv)
{
  int32_t fd_listen, n_avail, fd_max, n_sel, fd_conn, rc;
  ssize_t num;
  fd_set set_r, set_master;
  socklen_t addrlen, clilen;
  struct sockaddr* cliaddr = NULL;

  // init listen socket to port 27976
  fd_listen = _tcp_listen(NULL, "27976", &addrlen);
  // or, alternatively to host 10.0.2.15 and port 27976
  //fd_listen = _tcp_listen("10.0.2.15", "27976", &addrlen);

  // init select
  FD_ZERO(&set_master);
  FD_SET(fd_listen, &set_master);

  // TODO
  fd_max = fd_listen;

  cliaddr = _malloc(addrlen);
  n_avail = n_children;
  child_ptr = _calloc(n_children, sizeof(*child_ptr));
  
  // prefork all the children
  int32_t idx;
  for(idx=0; idx<n_children; ++idx){
    child_make(idx, fd_listen, addrlen); // only parent returns
    FD_SET(child_ptr[idx].fd_child_pipe, &set_master);
    fd_max = max(fd_max, child_ptr[idx].fd_child_pipe);
  }

  // init signal handler
  _signal(SIGINT, sig_int);

  while(1){
    set_r = set_master;
    if(0 >= n_avail){
      // turn off if no available children
      FD_CLR(fd_listen, &set_r); 
    }

    n_sel = _select( fd_max+1, &set_r, NULL, NULL, NULL);
    
    // check for new connections
    if(FD_ISSET(fd_listen, &set_r)){
      clilen = addrlen;
      fd_conn = _accept(fd_listen, cliaddr, &clilen);

      // get next available child index
      for(idx=0; idx<n_children; ++idx){
        if(child_ptr[idx].child_status == 0){
          break; // available
        }
      }

      // exit, in case index ran throu..
      if(idx == n_children){
        err_quit("no available children");
      }
    
      child_ptr[idx].child_status = 1; // mark child as busy
      ++child_ptr[idx].child_count;
      --n_avail;

      // TODO
      num = _write_fd(child_ptr[idx].fd_child_pipe, "", 1, fd_conn);

      // TODO
      _close(fd_conn);

      // TODO
      if(--n_sel == 0){
        continue; // all done with select() results
      }      
    }

    // find any newly-available children
    for(idx=0; idx<n_children; ++idx){
      if(FD_ISSET(child_ptr[idx].fd_child_pipe, &set_r)){
        if(0 == (num = _read(child_ptr[idx].fd_child_pipe, &rc, 1))){
          err_quit("child %d terminated unexepctedly", idx);
        }
        child_ptr[idx].child_status = 0;
        ++n_avail;
        if(--n_sel == 0){
          break; // all done with select() results
        }
      }
    }
  }
}

