// lib_pthread.c
/*
  pthreads wrapper functions.
*/

#include "lib_pthread.h"


void _pthread_create( pthread_t *tid
                      , const pthread_attr_t *attr
                      , void * (*func)(void *)
                      , void *arg)
{
  int  res;  
  if(0 == (res = pthread_create(tid, attr, func, arg))){
    return;
  }
  errno = res;
  err_sys("pthread_create error");
}



void _pthread_join(pthread_t tid
                   , void **status)
{
  int res;  
  if(0 == (res = pthread_join(tid, status))){
    return;
  }
  errno = res;
  err_sys("pthread_join error");
}



void _pthread_detach(pthread_t tid)
{
  int res;
  if(0 == (res = pthread_detach(tid))){
    return;
  }
  errno = res;
  err_sys("pthread_detach error");
}



void _pthread_kill(pthread_t tid, int signo)
{
  int  res;
  if( 0 == (res = pthread_kill(tid, signo))){
    return;
  }
  errno = res;
  err_sys("pthread_kill error");
}



void _pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
  int res;  
  if(0 == (res = pthread_mutexattr_init(attr))){
    return;
  }
  errno = res;
  err_sys("pthread_mutexattr_init error");
}



#ifdef _POSIX_THREAD_PROCESS_SHARED
void _pthread_mutexattr_setpshared(pthread_mutexattr_t *attr
                                   , int flag)
{
  int res;
  if(0 == (res = pthread_mutexattr_setpshared(attr, flag))){
    return;
  }
  errno = res;
  err_sys("pthread_mutexattr_setpshared error");
}
#endif  



void _pthread_mutex_init(pthread_mutex_t *mptr
                         , pthread_mutexattr_t *attr)
{
  int res;  
  if(0 == (res = pthread_mutex_init(mptr, attr))){
    return;
  }
  errno = res;
  err_sys("pthread_mutex_init error");
}



void _pthread_mutex_lock(pthread_mutex_t *mptr)
{
  int res;
  if(0 == (res = pthread_mutex_lock(mptr))){
    return;
  }
  errno = res;
  err_sys("pthread_mutex_lock error");
}



void _pthread_mutex_unlock(pthread_mutex_t *mptr)
{
  int res;
  if(0 == (res = pthread_mutex_unlock(mptr))){
    return;
  }
  errno = res;
  err_sys("pthread_mutex_unlock error");
}



void _pthread_cond_broadcast(pthread_cond_t *cptr)
{
  int res;
  if(0 == (res = pthread_cond_broadcast(cptr))){
    return;
  }
  errno = res;
  err_sys("pthread_cond_broadcast error");
}



void _pthread_cond_signal(pthread_cond_t *cptr)
{
  int res;
  if(0 == (res = pthread_cond_signal(cptr))){
    return;
  }
  errno = res;
  err_sys("pthread_cond_signal error");
}



void _pthread_cond_wait(pthread_cond_t *cptr
                        , pthread_mutex_t *mptr)
{
  int res;  
  if(0 == (res = pthread_cond_wait(cptr, mptr))){
    return;
  }
  errno = res;
  err_sys("pthread_cond_wait error");
}



void _pthread_cond_timedwait(pthread_cond_t *cptr
                             , pthread_mutex_t *mptr
                             , const struct timespec *tsptr)
{
  int res;
  if(0 == (res = pthread_cond_timedwait(cptr, mptr, tsptr))){
    return;
  }
  errno = res;
  err_sys("pthread_cond_timedwait error");
}



void _pthread_once(pthread_once_t *ptr
                   , void (*func)(void))
{
  int res;
  if(0 == (res = pthread_once(ptr, func))){
    return;
  }
  errno = res;
  err_sys("pthread_once error");
}



void _pthread_key_create(pthread_key_t *key
                         , void (*func)(void *))
{
  int res;  
  if(0 == (res = pthread_key_create(key, func))){
    return;
  }
  errno = res;
  err_sys("pthread_key_create error");
}



void _pthread_setspecific(pthread_key_t key
                          , const void *value)
{
  int res;
  if(0 == (res = pthread_setspecific(key, value))){
    return;
  }
  errno = res;
  err_sys("pthread_setspecific error");
}
