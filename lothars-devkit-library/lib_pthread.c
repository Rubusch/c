#include "lib_pthread.h"

/*
  The pthread_create() function starts a new thread in the calling
  process. The new thread starts execution by invoking
  start_routine(); arg is passed as the sole argument of
  start_routine().

  #include <pthread.h>

  @tid: The thread id.
  @attr: The optional thread attributes.
  @func: The routine function for the thread to create.
  @arg: The arguments for the routine function.
*/
void lothars__pthread_create( pthread_t *tid
			      , const pthread_attr_t *attr
			      , void * (*func)(void *)
			      , void *arg)
{
	int  res;
	if (0 != (res = pthread_create(tid, attr, func, arg))) {
		errno = res;
		err_sys("pthread_create error");
	}
}


/*
*/
void lothars__pthread_join(pthread_t tid
			   , void **status)
{
	int res;
	if (0 != (res = pthread_join(tid, status))) {
		errno = res;
		err_sys("pthread_join error");
	}
}


/*
*/
void lothars__pthread_detach(pthread_t tid)
{
	int res;
	if (0 == (res = pthread_detach(tid))) {
		errno = res;
		err_sys("pthread_detach error");
	}
}


/*
*/
void lothars__pthread_kill(pthread_t tid, int signo)
{
	int  res;
	if (0 != (res = pthread_kill(tid, signo))) {
		errno = res;
		err_sys("pthread_kill error");
	}
}


/*
*/
void lothars__pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	int res;
	if (0 != (res = pthread_mutexattr_init(attr))) {
		errno = res;
		err_sys("pthread_mutexattr_init error");
	}
}


/*
*/
void lothars__pthread_mutexattr_setpshared(pthread_mutexattr_t *attr
					   , int flag)
{
	int res;
	if (0 != (res = pthread_mutexattr_setpshared(attr, flag))) {
		errno = res;
		err_sys("pthread_mutexattr_setpshared error");
	}
}


/*
*/
void lothars__pthread_mutex_init(pthread_mutex_t *mptr
				 , pthread_mutexattr_t *attr)
{
	int res;
	if (0 != (res = pthread_mutex_init(mptr, attr))) {
		errno = res;
		err_sys("pthread_mutex_init error");
	}
}


/*
*/
void lothars__pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int res;
	if (0 != (res = pthread_mutex_lock(mptr))) {
		errno = res;
		err_sys("pthread_mutex_lock error");
	}
}


/*
*/
void lothars__pthread_mutex_unlock(pthread_mutex_t *mptr)
{
	int res;
	if (0 != (res = pthread_mutex_unlock(mptr))) {
		errno = res;
		err_sys("pthread_mutex_unlock error");
	}
}


/*
*/
void lothars__pthread_cond_broadcast(pthread_cond_t *cptr)
{
	int res;
	if (0 != (res = pthread_cond_broadcast(cptr))) {
		errno = res;
		err_sys("pthread_cond_broadcast error");
	}
}


/*
*/
void lothars__pthread_cond_signal(pthread_cond_t *cptr)
{
	int res;
	if (0 != (res = pthread_cond_signal(cptr))) {
		errno = res;
		err_sys("pthread_cond_signal error");
	}
}


/*
*/
void lothars__pthread_cond_wait(pthread_cond_t *cptr
				, pthread_mutex_t *mptr)
{
	int res;
	if (0 != (res = pthread_cond_wait(cptr, mptr))) {
		errno = res;
		err_sys("pthread_cond_wait error");
	}
}

/*
*/
void lothars__pthread_cond_timedwait(pthread_cond_t *cptr
				     , pthread_mutex_t *mptr
				     , const struct timespec *tsptr)
{
	int res;
	if (0 != (res = pthread_cond_timedwait(cptr, mptr, tsptr))) {
		errno = res;
		err_sys("pthread_cond_timedwait error");
	}
}


/*
*/
void lothars__pthread_once(pthread_once_t *ptr
			   , void (*func)(void))
{
	int res;
	if (0 != (res = pthread_once(ptr, func))) {
		errno = res;
		err_sys("pthread_once error");
	}
}


/*
*/
void lothars__pthread_key_create(pthread_key_t *key
				 , void (*func)(void *))
{
	int res;
	if (0 != (res = pthread_key_create(key, func))) {
		errno = res;
		err_sys("pthread_key_create error");
	}
}


/*
*/
void lothars__pthread_setspecific(pthread_key_t key
				  , const void *value)
{
	int res;
	if (0 != (res = pthread_setspecific(key, value))) {
		errno = res;
		err_sys("pthread_setspecific error");
	}
}
