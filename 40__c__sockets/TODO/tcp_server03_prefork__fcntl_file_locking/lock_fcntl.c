// fcntl_lock.c
/*
  int fcntl(int fd, int cmd, ... );

  fcntl() performs one of the operations described below on the open file
  descriptor fd. The operation is determined by cmd.

  fcntl() can take an optional third argument. Whether or not this argument is
  required is determined by cmd.

  locking with fnctl

  F_GETLK, F_SETLK and F_SETLKW are used to acquire, release, and test for the
  existence of record locks (also known as file-segment or file-region locks).

  man 2 fcntl
 */

#include <stdlib.h> // exit()
#include <stdio.h> // BUFSIZ, perror()
#include <string.h> // memset(), strncpy()

#include <errno.h> // errno, EINTR
#include <sys/types.h> // open()
#include <sys/stat.h> // S_IRUSR, S_IWUSR, S_IRGRP, S_IROTH
#include <unistd.h> // unlink()
#include <fcntl.h> // fcntl()

static struct flock lock_it;
static struct flock unlock_it;
static int lock_fd = -1; // fcntl command, BLOCKS as long as '-1'


void my_lock_init( char *pathname )
{
        char lock_file[BUFSIZ]; memset( lock_file, '\0', BUFSIZ );

        if (NULL == strncpy( lock_file, pathname, sizeof( lock_file ))) {
                perror( "pathname corrupt" );
                exit( EXIT_FAILURE );
        }

/*
  int mkstemp(char *template)

  The mkstemp() function generates a unique temporary filename from template,
  creates and opens the file, and returns an open file descriptor for the file.

  The last six characters of template must be "XXXXXX" and these are replaced
  with a string that makes the filename unique. Since it will be modified,
  template must not be a string constant, but should be declared as a character
  array.

  The file is created with permissions 0600, that is, read plus write for owner
  only.
 */
        fprintf( stderr, "\t%s:%d - lock_file '%s'\n", __FILE__, __LINE__, lock_file );
//*
        // mkstemp
        if (0 > (lock_fd = mkstemp( lock_file ))) {
                perror( "mkstemp failed" );
                exit( EXIT_FAILURE );
        }
/*/
        // mktemp - alternative, no mkstemp available
        //
        // CAUTION: mktemp is dangerous
        //
        // Never use mktemp(). Some implementations follow 4.3BSD and replace
        // XXXXXX by the current process ID and a single letter, so that at most
        // 26 different names can be returned. Since on the one hand the names
        // are easy to guess, and on the other hand there is a race between
        // testing whether the name exists and opening the file, every use of
        // mktemp() is a security risk. The race is avoided by mkstemp(3).
        //
        // * not using file descriptors
        // * not using FILE pointers
        // * not standard
        if (0 == lock_file[0] || NULL == mktemp( lock_file )) {
                perror( "mktemp failed" );
                exit( EXIT_FAILURE );
        }

        // default file access permissions for new files
        // S_IRUSR    00400 user has read permission
        // S_IWUSR    00200 user has write permission
        // S_IXGRP    00010 group has execute permission
        // S_IROTH    00004 others have read permission
        mode_t file_mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        // create file, touch
        // O_CREAT    00000100
        // O_WRONLY   00000001
        if (0 > (lock_fd = open( lock_file, O_CREAT | O_WRONLY, file_mode ))) {
                fprintf( stderr, "open failed for '%s'\n", lock_file );
                exit( EXIT_FAILURE );
        }
//*/
        fprintf( stderr, "\t%s:%d - lock_fd '%d'\n", __FILE__, __LINE__, lock_fd );


/*
  unlink() deletes a name from the file system. If that name was the last link
  to a file and no processes have the file open the file is deleted and the
  space it was using is made available for reuse.
*/
        // unlink
        if (0 > unlink( lock_file )) {
                fprintf( stderr, "unlink failed for '%s'\n", lock_file );
                exit( EXIT_FAILURE );
        }

/*
  lock, is a pointer to a structure that has at least the following fields (in
  unspecified order).

  struct flock {
          ...
          short l_type;    // Type of lock: F_RDLCK, F_WRLCK, F_UNLCK
          short l_whence;  // How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END
          off_t l_start;   // Starting offset for lock
          off_t l_len;     // Number of bytes to lock
          pid_t l_pid;     // PID of process blocking our lock (F_GETLK only)
          ...
  };

  The l_whence, l_start, and l_len fields of this structure specify the range of
  bytes we wish to lock. Bytes past the end of the file may be locked, but not
  bytes before the start of the file.

  l_start is the starting offset for the lock, and is interpreted relative to
  either: the start of the file (if l_whence is SEEK_SET); the current file
  offset (if l_whence is SEEK_CUR); or the end of the file (if l_whence is
  SEEK_END). In the final two cases, l_start can be a negative number provided
  the offset does not lie before the start of the file.

  l_len specifies the number of bytes to be locked. If l_len is positive, then
  the range to be locked covers bytes l_start up to and including
  l_start + l_len-1. Specifying 0 for l_len has the special meaning: lock all
  bytes starting at the location specified by l_whence and l_start through to
  the end of file, no matter how large the file grows.

  POSIX.1-2001 allows (but does not require) an implementation to support a
  negative l_len value; if l_len is negative, the interval described by lock
  covers bytes l_start+l_len up to and including l_start-1. This is supported
  by Linux since kernel versions 2.4.21 and 2.5.49.

  The l_type field can be used to place a read (F_RDLCK) or a write (F_WRLCK)
  lock on a file. Any number of processes may hold a read lock (shared lock) on
  a file region, but only one process may hold a write lock (exclusive lock). An
  exclusive lock excludes all other locks, both shared and exclusive. A single
  process can hold only one type of lock on a file region; if a new lock is
  applied to an already-locked region, then the existing lock is converted to
  the new lock type. (Such conversions may involve splitting, shrinking, or
  coalescing with an existing lock if the byte range specified by the new lock
  does not precisely coincide with the range of the existing lock.)
*/
        // locks - init static structure instance for locks
        lock_it.l_type = F_WRLCK;
        lock_it.l_whence = SEEK_SET;
        lock_it.l_start = 0;
        lock_it.l_len = 0;

        // unlocks - init static structure instance for unlocks
        unlock_it.l_type = F_UNLCK;
        unlock_it.l_whence = SEEK_SET;
        unlock_it.l_start = 0;
        unlock_it.l_len = 0;
}


/*
  lock
 */
void my_lock_wait()
{
        int rc;

/*
  F_SETLKW (struct flock *)

  As for F_SETLK, but if a conflicting lock is held on the file, then wait for
  that lock to be released. If a signal is caught while waiting, then the call
  is interrupted and (after the signal handler has returned) returns immediately
  (with return value -1 and errno set to EINTR; see signal(7)).
 */

        // fnctl, may BLOCK
        while (0 > (rc = fcntl( lock_fd, F_SETLKW, &lock_it))) {
                if (errno == EINTR) {
                        continue;
                }
                perror( "ERROR: fcntl failed for my_lock_wait" );
                break;
        }
}


/*
  unlock
 */
void my_lock_release()
{
        // fcntl, may BLOCK
        if (0 > fcntl( lock_fd, F_SETLKW, &unlock_it)) {
                perror( "ERROR: fcntl failed for my_lock_release" );
        }
}
