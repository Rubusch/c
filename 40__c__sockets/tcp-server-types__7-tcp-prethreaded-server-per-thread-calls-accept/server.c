// server.c
/*
  TCP Prethreaded Server, per-Thread accept()

  faster than concurrent server - one of the fastest implementations

  TODO check (calloc, put cliaddr individually into a thread struct 
  for each thread) 
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


pthread_mutex_t mx_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
  pthread_t thread_tid;
  long thread_count;
} Thread_t;
Thread_t* thread_ptr;

int32_t fd_listen, n_threads;
socklen_t addrlen;


/*
  example code: print system time
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
  example code: mini signal handler printing system time when shutting down
//*/
void sig_int(int32_t signo)
{
  pr_cpu_time();
  if(NULL != thread_ptr){
    free(thread_ptr);
    thread_ptr = NULL;
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

    // get as long
    n_towrite = atol(line);

    // check if valid
    if((0 >= n_towrite) || (MAXN < n_towrite)){
      err_quit("client request for %d bytes", n_towrite);
    }
    
    // if ok, write to socket
    _write(fd_sock, result, n_towrite);
  }
}


/*
  thread behavior, calls the accept (within the thread, using mutexes)
//*/
void* thread_main(void* arg)
{
  int32_t fd_conn;
  socklen_t clilen;

  struct sockaddr* cliaddr = _malloc(addrlen); // TODO
  printf("thread %d starting\n", (int32_t) arg);
  while(1){
    clilen = addrlen;

    // lock
    _pthread_mutex_lock(&mx_lock);    

    // accept
    fd_conn = _accept(fd_listen, cliaddr, &clilen);

    // unlock
    _pthread_mutex_unlock(&mx_lock);

    
    ++thread_ptr[(int32_t) arg].thread_count;

    child_routine(fd_conn); // process request

    _close(fd_conn);
  }
}


/*
  creates thread, calls thread_main()
//*/
void thread_make(int32_t idx)
{
  _pthread_create(&thread_ptr[idx].thread_tid, NULL, &thread_main, (void*) idx);
}


/*
  main
//*/
int32_t main(int32_t argc, char** argv)
{
  // listen on port 27976
  fd_listen = _tcp_listen(NULL, "27976", &addrlen);
  /* alternatively listen on host port: 10.0.2.2
  fd_listen = _tcp_listen("10.0.2.2", "27976", &addrlen);
  //*/

  // set number of threads: 7
  n_threads = 7;
  thread_ptr = _calloc(n_threads, sizeof(*thread_ptr));

  // start all threads
  int32_t idx;
  for(idx=0; idx<n_threads; ++idx){
    thread_make(idx); // only main thread returns
  }

  // main thread returns and signal handler only should print number of threads
  _signal(SIGINT, sig_int);
  
  while(1){ 
    pause(); // everything done by threads
  }
}


