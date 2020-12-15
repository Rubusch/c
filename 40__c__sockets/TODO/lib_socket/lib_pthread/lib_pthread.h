// lib_pthread.h
/* 
   pthread wrapper

   based on Stevens unix network programming (1996)
*/

#ifndef __lib_pthread_h
#define __lib_pthread_h

#include "../lib_socket.h"


void _pthread_create(pthread_t *, const pthread_attr_t *, void * (*)(void *), void *);
void _pthread_join(pthread_t, void **);
void _pthread_detach(pthread_t);
void _pthread_kill(pthread_t, int);

void _pthread_mutexattr_init(pthread_mutexattr_t *);
void _pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
void _pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
void _pthread_mutex_lock(pthread_mutex_t *);
void _pthread_mutex_unlock(pthread_mutex_t *);

void _pthread_cond_broadcast(pthread_cond_t *);
void _pthread_cond_signal(pthread_cond_t *);
void _pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
void _pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);

void _pthread_key_create(pthread_key_t *, void (*)(void *));
void _pthread_setspecific(pthread_key_t, const void *);
void _pthread_once(pthread_once_t *, void (*)(void));

#endif
