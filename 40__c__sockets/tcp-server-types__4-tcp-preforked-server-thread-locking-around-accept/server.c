// server.c
/*
  TCP preforked server, thread locking around accept

  (with thread mutex locking to protect accept)
//*/

#include "../lib_socket/lib_socket.h"
#include "../lib_socket/lib_pthread/lib_pthread.h"
#include <sys/mman.h>


static int32_t n_children;
static pid_t *pid_children;

// mutex pointer
static pthread_mutex_t *mx_ptr;


/*
  init pthread mutexes 
//*/
void lock_init(char* pathname)
{
  pthread_mutexattr_t mxattr;
  
  int32_t fd = _open("/dev/zero", O_RDWR, 0);
  mx_ptr = _mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  _close(fd);
  
  _pthread_mutexattr_init(&mxattr);
  _pthread_mutexattr_setpshared(&mxattr, PTHREAD_PROCESS_SHARED);
  _pthread_mutex_init(mx_ptr, &mxattr);
}


/*
  lock blocking
//*/
void lock_wait()
{
  _pthread_mutex_lock(mx_ptr);
}


/*
  release lock
//*/
void lock_release()
{
  _pthread_mutex_unlock(mx_ptr);
}


/*
  print user time and server time when shutting down (SIGTERM caught)
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


// max character number
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

    // mutex lock
    lock_wait();

    fd_conn = _accept(fd_listen, cliaddr, &clilen);

    // mutex unlock
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
  // ..or alternatively, listen on ip 10.0.2.15, on port 17976
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
