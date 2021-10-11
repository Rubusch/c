#ifndef DEVKIT_LIB_PTHREAD
#define DEVKIT_LIB_PTHREAD

/*
  pthreads wrapper functions

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/

/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h> /* fcntl(), F_GETFL */
#include <pthread.h>

#include "lib_error.h"

/* forwards */

void lothars__pthread_create(pthread_t *, const pthread_attr_t *,
			     void *(*)(void *), void *);
void lothars__pthread_join(pthread_t, void **);
void lothars__pthread_detach(pthread_t);
void lothars__pthread_kill(pthread_t, int);
void lothars__pthread_mutexattr_init(pthread_mutexattr_t *);
void lothars__pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
void lothars__pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
void lothars__pthread_mutex_lock(pthread_mutex_t *);
void lothars__pthread_mutex_unlock(pthread_mutex_t *);
void lothars__pthread_cond_broadcast(pthread_cond_t *);
void lothars__pthread_cond_signal(pthread_cond_t *);
void lothars__pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
void lothars__pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
				     const struct timespec *);
void lothars__pthread_key_create(pthread_key_t *, void (*)(void *));
void lothars__pthread_setspecific(pthread_key_t, const void *);
void lothars__pthread_once(pthread_once_t *, void (*)(void));

#endif /* DEVKIT_LIB_PTHREAD */
