// server.h
/*
  tcp prethreaded server, main thread calls accept
//*/

typedef struct{
  pthread_t thread_tid; // thread id
  long thread_count; // number of connections handled
} Thread_t;

Thread_t *thread_ptr;


// max number of threads running
#define MAXNCLI 32

// array of socket descriptors
int fd_cli[MAXNCLI]

  // thread index
  , idx_thread

  // main thread index (only one main thread!)
  , idx_mainthread;

pthread_mutex_t mutex_fd_cli;
pthread_cond_t cond_fd_cli;


// forward declarations to avoid warnings
void thread_make(int); 

