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
  The pthread_join() function waits for the thread specified by thread
  to terminate. If that thread has already terminated, then
  pthread_join() returns immediately. The thread specified by thread
  must be joinable.

  #include <pthread.h>

  @tid: The thread to wait for termination (thread).
  @status: If status (retval) is not NULL, then pthread_join() copies
      the exit status of the target thread (i.e., the value that the
      target thread supplied to pthread_exit(3)) into the location
      pointed to by *status.
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
  The pthread_detach() function marks the thread identified by thread
  as detached. When a detached thread terminates, its resources are
  automatically released back to the system without the need for
  another thread to join with the terminated thread.

  #include <pthread.h>

  @tid: The thread id to detach (thread).
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
  The pthread_kill() function sends the signal sig to thread, a thread
  in the same process as the caller. The signal is asynchronously
  directed to thread.

  If sig is 0, then no signal is sent, but error checking is still
  performed; this can be used to check for the existence of a thread
  ID.

  #include <signal.h>

  @tid: The thread to send the signal to (thread).
  @signo: The signal number to send (sig).
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
  The pthread_mutexattr_init() function shall initialize a mutex
  attributes object attr with the default value for all of the
  attributes defined by the implementation.

  Results are undefined if pthread_mutexattr_init() is called
  specifying an already initialized attr attributes object.

  #include <pthread.h>

  @attr: A mutex attributes object with default values.
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
  The pthread_mutexattr_setpshared() function shall set the
  process-shared attribute in an initialized attributes object
  referenced by attr.

  The process-shared attribute is set to PTHREAD_PROCESS_SHARED to
  permit a mutex to be operated upon by any thread that has access to
  the memory where the mutex is allocated, even if the mutex is
  allocated in memory that is shared by multiple processes. If the
  process-shared attribute is PTHREAD_PROCESS_PRIVATE, the mutex shall
  only be operated upon by threads created within the same process as
  the thread that initialized the mutex; if threads of differing
  processes attempt to operate on such a mutex, the behavior is
  undefined. The default value of the attribute shall be
  PTHREAD_PROCESS_PRIVATE.

  #include <pthread.h>

  @attr: The mutex attributes object to pass the process-shared
      attribute.
  @flag: The process-shared attribute value to set.
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
  The pthread_mutex_init() function shall initialize the mutex
  referenced by mutex with attributes specified by attr. If attr is
  NULL, the default mutex attributes are used; the effect shall be the
  same as passing the address of a default mutex attributes
  object. Upon successful initialization, the state of the mutex
  becomes initialized and unlocked.

  #include <pthread.h>

  @mptr: The mutex to be initialized (mutex).
  @attr: The attribute for the mutex to be initialized.
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
  The mutex object referenced by mutex shall be locked by calling
  pthread_mutex_lock(). If the mutex is already locked, the calling
  thread shall block until the mutex becomes available. This operation
  shall return with the mutex object referenced by mutex in the locked
  state with the calling thread as its owner.

  If the mutex type is PTHREAD_MUTEX_NORMAL, deadlock detection shall
  not be provided. Attempting to relock the mutex causes deadlock. If
  a thread attempts to unlock a mutex that it has not locked or a
  mutex which is unlocked, undefined behavior results.

  If the mutex type is PTHREAD_MUTEX_ERRORCHECK, then error checking
  shall be provided. If a thread attempts to relock a mutex that it
  has already locked, an error shall be returned. If a thread attempts
  to unlock a mutex that it has not locked or a mutex which is
  unlocked, an error shall be returned.

  If the mutex type is PTHREAD_MUTEX_RECURSIVE, then the mutex shall
  maintain the concept of a lock count. When a thread successfully
  acquires a mutex for the first time, the lock count shall be set to
  one. Every time a thread relocks this mutex, the lock count shall be
  incremented by one. Each time the thread unlocks the mutex, the lock
  count shall be decremented by one. When the lock count reaches zero,
  the mutex shall become available for other threads to acquire. If a
  thread attempts to unlock a mutex that it has not locked or a mutex
  which is unlocked, an error shall be returned.

  If the mutex type is PTHREAD_MUTEX_DEFAULT, attempting to
  recursively lock the mutex results in undefined behavior. Attempting
  to unlock the mutex if it was not locked by the calling thread
  results in undefined behavior. Attempting to unlock the mutex if it
  is not locked results in undefined behavior.

  #include <pthread.h>

  @mptr: The specified mutex.
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
  The mutex object referenced by mutex shall be locked by calling
  pthread_mutex_lock(). If the mutex is already locked, the calling
  thread shall block until the mutex becomes available. This operation
  shall return with the mutex object referenced by mutex in the locked
  state with the calling thread as its owner.

  If the mutex type is PTHREAD_MUTEX_NORMAL, deadlock detection shall
  not be provided. Attempting to relock the mutex causes deadlock. If
  a thread attempts to unlock a mutex that it has not locked or a
  mutex which is unlocked, undefined behavior results.

  If the mutex type is PTHREAD_MUTEX_ERRORCHECK, then error checking
  shall be provided. If a thread attempts to relock a mutex that it
  has already locked, an error shall be returned. If a thread attempts
  to unlock a mutex that it has not locked or a mutex which is
  unlocked, an error shall be returned.

  If the mutex type is PTHREAD_MUTEX_RECURSIVE, then the mutex shall
  maintain the concept of a lock count. When a thread successfully
  acquires a mutex for the first time, the lock count shall be set to
  one. Every time a thread relocks this mutex, the lock count shall be
  incremented by one. Each time the thread unlocks the mutex, the lock
  count shall be decremented by one. When the lock count reaches zero,
  the mutex shall become available for other threads to acquire. If a
  thread attempts to unlock a mutex that it has not locked or a mutex
  which is unlocked, an error shall be returned.

  If the mutex type is PTHREAD_MUTEX_DEFAULT, attempting to
  recursively lock the mutex results in undefined behavior. Attempting
  to unlock the mutex if it was not locked by the calling thread
  results in undefined behavior. Attempting to unlock the mutex if it
  is not locked results in undefined behavior.

  The pthread_mutex_unlock() function shall release the mutex object
  referenced by mutex. The manner in which a mutex is released is
  dependent upon the mutex's type attribute. If there are threads
  blocked on the mutex object referenced by mutex when
  pthread_mutex_unlock() is called, resulting in the mutex becoming
  available, the scheduling policy shall determine which thread shall
  acquire the mutex.

  #include <pthread.h>

  @mptr: The specified mutex.
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
  These functions shall unblock threads blocked on a condition
  variable.

  The pthread_cond_broadcast() function shall unblock all threads
  currently blocked on the specified condition variable cond.

  If more than one thread is blocked on a condition variable, the
  scheduling policy shall determine the order in which threads are
  unblocked. When each thread unblocked as a result of a
  pthread_cond_broadcast() or pthread_cond_signal() returns from its
  call to pthread_cond_wait() or pthread_cond_timedwait(), the thread
  shall own the mutex with which it called pthread_cond_wait() or
  pthread_cond_timedwait(). The thread(s) that are unblocked shall
  contend for the mutex according to the scheduling policy (if
  applicable), and as if each had called pthread_mutex_lock().

  #include <pthread.h>

  @cptr: Points to the conditional variable (cond).
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
  These functions shall unblock threads blocked on a condition
  variable.

  The pthread_cond_signal() function shall unblock at least one of the
  threads that are blocked on the specified condition variable cond
  (if any threads are blocked on cond).

  #include <pthread.h>

  @cptr: Points to the conditional variable (cond).
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
  The pthread_cond_timedwait() and pthread_cond_wait() functions shall
  block on a condition variable. They shall be called with mutex
  locked by the calling thread or undefined behavior results.

  These functions atomically release mutex and cause the calling
  thread to block on the condition variable cond; atomically here
  means "atomically with respect to access by another thread to the
  mutex and then the condition variable". That is, if another thread
  is able to acquire the mutex after the about-to-block thread has
  released it, then a subsequent call to pthread_cond_broadcast() or
  pthread_cond_signal() in that thread shall behave as if it were
  issued after the about-to-block thread has blocked.

  #include <pthread.h>

  @cptr: Points to the specified conditional variable (cond).
  @mptr: The mutex to be locked at call.
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
  The pthread_cond_timedwait() and pthread_cond_wait() functions shall
  block on a condition variable. They shall be called with mutex
  locked by the calling thread or undefined behavior results.

  These functions atomically release mutex and cause the calling
  thread to block on the condition variable cond; atomically here
  means "atomically with respect to access by another thread to the
  mutex and then the condition variable". That is, if another thread
  is able to acquire the mutex after the about-to-block thread has
  released it, then a subsequent call to pthread_cond_broadcast() or
  pthread_cond_signal() in that thread shall behave as if it were
  issued after the about-to-block thread has blocked.

  The pthread_cond_timedwait() function shall be equivalent to
  pthread_cond_wait(), except that an error is returned if the
  absolute time specified by abstime passes (that is, system time
  equals or exceeds abstime) before the condition cond is signaled or
  broadcasted, or if the absolute time specified by abstime has
  already been passed at the time of the call.

  #include <pthread.h>

  @cptr: Points to the specified conditional variable (cond).
  @mptr: The mutex to be locked at call.
  @tsptr: The timer object.
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
  The first call to pthread_once() by any thread in a process, with a
  given once_control, shall call the init_routine with no
  arguments. Subsequent calls of pthread_once() with the same
  once_control shall not call the init_routine. On return from
  pthread_once(), init_routine shall have completed. The once_control
  parameter shall determine whether the associated initialization
  routine has been called.

  The pthread_once() function is not a cancellation point. However, if
  init_routine is a cancellation point and is canceled, the effect on
  once_control shall be as if pthread_once() was never called.

  #include <pthread.h>

  @ptr: The once control object (once_control).
  @func: The function to call w/o any arguments (init_routine).
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
  The pthread_key_create() function shall create a thread-specific
  data key visible to all threads in the process. Key values provided
  by pthread_key_create() are opaque objects used to locate
  thread-specific data. Although the same key value may be used by
  different threads, the values bound to the key by
  pthread_setspecific() are maintained on a per-thread basis and
  persist for the life of the calling thread.

  Upon key creation, the value NULL shall be associated with the new
  key in all active threads. Upon thread creation, the value NULL
  shall be associated with all defined keys in the new thread.

  An optional destructor function may be associated with each key
  value. At thread exit, if a key value has a non-NULL destructor
  pointer, and the thread has a non-NULL value associated with that
  key, the value of the key is set to NULL, and then the function
  pointed to is called with the previously associated value as its
  sole argument. The order of destructor calls is unspecified if more
  than one destructor exists for a thread when it exits.

  If, after all the destructors have been called for all non-NULL
  values with associated destructors, there are still some non-NULL
  values with associated destructors, then the process is
  repeated. If, after at least {PTHREAD_DESTRUCTOR_ITERATIONS}
  iterations of destructor calls for outstanding non-NULL values,
  there are still some non-NULL values with associated destructors,
  implementations may stop calling destructors, or they may continue
  calling destructors until no non-NULL values with associated
  destructors exist, even though this might result in an infinite
  loop.

  #include <pthread.h>

  @key: The key to be created.
  @func: The optional destructor to be provided.
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
  The pthread_setspecific() function shall associate a thread-specific
  value with a key obtained via a previous call to
  pthread_key_create(). Different threads may bind different values to
  the same key. These values are typically pointers to blocks of
  dynamically allocated memory that have been reserved for use by the
  calling thread.

  #include <pthread.h>

  @key: The obtained key to be provided.
  @value: The value to pass, typicall a pointer to dynamically
      allocated memory.
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
