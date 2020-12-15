// lock_pthread.c
/*
 */

#include <stdlib.h> // exit()
#include <stdio.h> // perror()

#include <unistd.h> // close(), pause()
#include <fcntl.h> // open()
#include <sys/mman.h> // mmap()
#include <errno.h> // errno

// check Makefile option: -lm -pthread
#include <pthread.h> // pthreads

static pthread_mutex_t *mptr; // actual mutex will be in shared memory


void my_lock_init( char* pathname )
{
        int fd = -1;
        int num = -1;
        pthread_mutexattr_t mattr;

        // open fd
        if (0 > (fd = open( "/dev/zero", O_RDWR, 0 ))) {
                fprintf( stderr, "\t%s:%d - open error for '%s'\n"
                         , __FILE__, __LINE__, "/dev/zero" );
                exit( EXIT_FAILURE );
        }

        // mmap mptr, with fd
        if (NULL == (mptr = mmap( 0
                                  , sizeof( *mptr )
                                  , PROT_READ | PROT_WRITE
                                  , MAP_SHARED
                                  , fd
                                  , 0
                             ))) {
                perror( "mmap failed" );
                exit( EXIT_FAILURE );
        }

        // close fd
        if (-1 == close( fd )) {
                perror( "close failed" );
                exit( EXIT_FAILURE );
        }

        // pthread - mutex_attr init
        if (0 != (num = pthread_mutexattr_init( &mattr ))) {
                perror( "pthread_mutexattr_init failed" );
                errno = num;
                return;
        }

        // pthread - mutex_attr set shared
        if (0 != pthread_mutexattr_setpshared( &mattr, PTHREAD_PROCESS_SHARED )) {
                perror( "pthread_mutexattr_setpshared failed" );
                errno = num;
                return;
        }

        // pthread - mutex init
        if (0 != pthread_mutex_init( mptr, &mattr )) {
                perror( "pthread_mutex_init failed" );
                errno = num;
                return;
        }
}

/*
  If the mutex type is PTHREAD_MUTEX_NORMAL, deadlock detection shall not be
  provided. Attempting to relock the mutex causes deadlock. If a thread attempts
  to unlock a mutex that it has not locked or a mutex which is unlocked,
  undefined behavior results.

  If the mutex type is PTHREAD_MUTEX_ERRORCHECK, then error checking shall be
  provided. If a thread attempts to relock a mutex that it has already locked,
  an error shall be returned.
 */
void my_lock_wait()
{
        puts( "WAIT for lock" ); // debugging

        int num = -1;
        if (0 != (num = pthread_mutex_lock( mptr ))) {
                perror( "pthread_mutex_lock failed" );
                errno = num;
                return;
        }

        puts( "LOCKED" ); // debugging
}

void my_lock_release()
{
        puts( "WAIT to unlock" ); // debugging

        int num = -1;
        if (0 != (num = pthread_mutex_unlock( mptr ))) {
                perror( "pthread_mutex_unlock failed" );
                errno = num;
                return;
        }

        puts( "UNLOCKED" );
}
