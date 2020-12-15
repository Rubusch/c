// thread.c
/*
  thread function used in the prethreaded tcp server application 
  (main thread calls accept)
//*/

#include "../lib_socket/lib_pthread/lib_pthread.h"
#include "thread.h"


// forward declaration (example code)
void web_child(int fd_sock);


/*
  thread main function called by phtread_create()
*/
void* thread_main(void* arg)
{
  int fd_conn;
  printf("thread %d starting\n", (int) arg);

  for(;;){

    _pthread_mutex_lock(&mutex_fd_cli);

    // check if thread index comes to main thread index -> wait
    while(idx_thread == idx_mainthread){
      _pthread_cond_wait(&cond_fd_cli, &mutex_fd_cli);
    }

    // connected socket to service
    fd_conn = fd_cli[idx_thread]; 

    // increment idx_thread, go to next thread
    if(++idx_thread == MAXNCLI) idx_thread = 0;
    
    _pthread_mutex_unlock(&mutex_fd_cli);
   

    ++thread_ptr[(int) arg].thread_count;
    
    // process request
    web_child(fd_conn); 

    _close(fd_conn);
  }
}


/*
  only main thread returns
*/
void thread_make(int idx)
{
  _pthread_create(&thread_ptr[idx].thread_tid, NULL, &thread_main, (void*) idx);
}
