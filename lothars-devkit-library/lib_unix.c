/*
  Linux / Unix / POSIX calls

  Wrapper collection for common Linux library functions.

  Lothar Rubusch, GPL

  ---
  References:
  Unix Network Programming, Stevens
  Unix Interprocess Communication, Stevens
*/


#include "lib_unix.h"

/*
  The calloc() function allocates memory for an array of nmemb
  elements of size bytes each and returns a pointer to the allocated
  memory. The memory is set to zero. If nmemb or size is 0, then
  calloc() returns either NULL, or a unique pointer value that can
  later be successfully passed to free().

  #include <stdlib.h>

  @nmemb: Number of elements
  @size: Size of element
*/
void* lothars__calloc(size_t nmemb, size_t size)
{
	void *ptr = NULL;
	if (NULL == (ptr = calloc(nmemb, size))) {
		err_sys("%s() error", __func__);
	}
	return ptr;
}


// TODO rm, prefer the version to set null after close
///*
//  The close() function shall deallocate the file descriptor indicated
//  by fd. To deallocate means to make the file descriptor available for
//  return by subsequent calls to open() or other functions that
//  allocate file descriptors. All outstanding record locks owned by the
//  process on the file associated with the file descriptor shall be
//  removed (that is, unlocked).
//
//  If close() is interrupted by a signal that is to be caught, it shall
//  return -1 with errno set to [EINTR] and the state of fildes is
//  unspecified. If an I/O error occurred while reading from or writing
//  to the file system during close(), it may return -1 with errno set
//  to [EIO]; if this error is returned, the state of fildes is
//  unspecified.
//
//  #include <unistd.h>
//
//  @fd: The file descriptor to the specific connection.
//*/
//void lothars__close(int fd)
//{
//	if (-1 == close(fd)) {
//		err_sys("%s() error", __func__);
//	}
//}

/*
  The close() function shall deallocate the file descriptor indicated
  by fd. To deallocate means to make the file descriptor available for
  return by subsequent calls to open() or other functions that
  allocate file descriptors. All outstanding record locks owned by the
  process on the file associated with the file descriptor shall be
  removed (that is, unlocked).

  If close() is interrupted by a signal that is to be caught, it shall
  return -1 with errno set to [EINTR] and the state of fildes is
  unspecified. If an I/O error occurred while reading from or writing
  to the file system during close(), it may return -1 with errno set
  to [EIO]; if this error is returned, the state of fildes is
  unspecified.

  This wrapper sets the fp to NULL;

  #include <unistd.h>

  @fd: Points to the file descriptor to the specific connection.
*/
void lothars__close_null(int *fd)
{
	if (NULL == fd) {
		fprintf(stderr, "%s() fd was NULL\n", __func__);
		return;
	}
	if (-1 == close(*fd)) {
		err_sys("%s() error", __func__);
	}
	*fd = 0;
	sync();
}



/*
  dup, dup2 - duplicate an open file descriptor. The dup() and dup2()
  functions provide an alternative interface to the service provided
  by fcntl() using the F_DUPFD command. The call:

    fid = dup(fildes);

  shall be equivalent to:

    fid = fcntl(fildes, F_DUPFD, 0);

  The call:

    fid = dup2(fildes, fildes2);

  shall be equivalent to:

    close(fildes2);
    fid = fcntl(fildes, F_DUPFD, fildes2);


  #include <unistd.h>

  @fd1: The original file descriptor.
  @fd2: The copy of the original file descriptor.
*/
void lothars__dup2(int fd1, int fd2)
{
	if (-1 == dup2(fd1, fd2)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The fcntl() function shall perform the operations described below on
  open files. The fildes argument is a file descriptor.

  #include <unistd.h>
  #include <fcntl.h>

  @fd: The file descriptor.
  @cmd: The fcntl command (see manpages).
  @arg: One additional int argument instead of variadic arguments in
      the fcntl() call.
*/
int lothars__fcntl(int fd, int cmd, int arg)
{
	int res;
	if (-1 == (res = fcntl(fd, cmd, arg))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  The gettimeofday() function shall obtain the current time, expressed
  as seconds and microseconds since the Epoch, and store it in the
  timeval structure pointed to by tv. The resolution of the system
  clock is unspecified.

  #include <sys/time.h>

  @tv: Current time is stored in *tv.
*/
void lothars__gettimeofday(struct timeval *tv)
{
	if (-1 == gettimeofday(tv, NULL)) { // if 'tzp' is not a NULL pointer, the behavior is unspecified
		err_sys("%s() error", __func__);
	}
}


/*
  The ioctl() function shall perform a variety of control functions on
  STREAMS devices.

  #include <stropts.h>

  @fd: The file descriptor on the stream.
  @request: The ioctl() request (see manpages).
  @arg: An optional argument.
*/
int lothars__ioctl(int fd, int request, void *arg)
{
	int  res;
	if (-1 == (res = ioctl(fd, request, arg))) {
		err_sys("%s() error", __func__);
	}
	return res; // streamio of I_LIST returns value
}


/*
  The fork() function shall create a new process.

  #include <unistd.h>
*/
pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("%s() error", __func__);
	}
	return pid;
}


/*
  The malloc() function allocates size bytes and returns a pointer to
  the allocated memory. The memory is not initialized. If size is 0,
  then malloc() returns either NULL, or a unique pointer value that
  can later be successfully passed to free().

  #include <stdlib.h>

  @size: Size of the memory in byte to allocate.
*/
void* lothars__malloc(size_t size)
{
	void *ptr;
	if (NULL == (ptr = malloc(size))) {
		err_sys("%s() error", __func__);
	}
	return ptr;
}


/*
  The mkstemp() function generates a unique temporary filename from
  template, creates and opens the file, and returns an open file
  descriptor for the file.

  The last six characters of template must be "XXXXXX" and these are
  replaced with a string that makes the filename unique. Since it will
  be modified, template must not be a string constant, but should be
  declared as a character array.

  might need
    _BSD_SOURCE || _SVID_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
  since glibc 2.12, also:
    || _POSIX_C_SOURCE >= 200112L

  #include <stdlib.h>

  @template: The filename.
*/
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
int lothars__mkstemp(char *template)
{
	int idx;

#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE) || _XOPEN_SOURCE >= 500 || defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED) || _POSIX_C_SOURCE >= 200112L
	if (0 > (idx = mkstemp(template))) {
		err_quit("mkstemp error, returned -1");
	}
#else
	if((mktemp(template) == NULL) || (template[0] == 0)){
		err_quit("mktemp error, NULL or not defined");
	}

	idx = lothars__open(template, O_CREAT | O_WRONLY, FILE_MODE);
#endif
	return idx;
}


/*
  The mmap() function shall establish a mapping between a process'
  address space and a file, shared memory object, or typed memory
  object. The format of the call is as follows:

    pa=mmap(addr, len, prot, flags, fildes, off);

  The mmap() function shall establish a mapping between the address
  space of the process at an address pa for len bytes to the memory
  object represented by the file descriptor fildes at offset off for
  len bytes. The value of pa is an implementation-defined function of
  the parameter addr and the values of flags, further described
  below. A successful mmap() call shall return pa as its result. The
  address range starting at pa and continuing for len bytes shall be
  legitimate for the possible (not necessarily current) address space
  of the process. The range of bytes starting at off and continuing
  for len bytes shall be legitimate for the possible (not necessarily
  current) offsets in the file, shared memory object, or typed memory
  object represented by fildes.

  #include <sys/mman.h>

  @addr: The address space of the process to be mapped.
  @len: The length of the address space of the process to be mapped.
  @prot: The parameter prot determines whether read, write, execute,
      or some combination of accesses are permitted.
  @flags: The parameter flags provides other information about the
      handling of the mapped data (e.g. MAP_PRIVATE or MAP_FIXED).
  @fd: A filedescriptor to the memory object to be mapped to.
  @offset: The offset of the memory object to be mapped to for len bytes.

  Returns the memory object pa as a result.
*/
void* lothars__mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void *ptr;
	if (((void *) -1) == (ptr = mmap(addr, len, prot, flags, fd, offset))) {
		err_sys("%s() error", __func__);
	}
	return ptr;
}


/*
  The open() function shall establish the connection between a file
  and a file descriptor. It shall create an open file description that
  refers to a file and a file descriptor that refers to that open file
  description. The file descriptor is used by other I/O functions to
  refer to that file. The path argument points to a pathname naming
  the file.

  #include <sys/stat.h>
  #include <fcntl.h>

  @pathname: The path to the file.
  @oflag: The flags for opening the specified file.
  @mode: The mode how to open the specified file.

  Returns the file descriptor.
*/
int lothars__open(const char *pathname, int oflag, mode_t mode)
{
	int fd;
	if (-1 == (fd = open(pathname, oflag, mode))) {
		err_sys("%s() error for %s", __func__, pathname);
	}
	return fd;
}


/*
  The pipe() function shall create a pipe and place two file
  descriptors, one each into the arguments fildes[0] and fildes[1],
  that refer to the open file descriptions for the read and write ends
  of the pipe. Their integer values shall be the two lowest available
  at the time of the pipe() call. The O_NONBLOCK and FD_CLOEXEC flags
  shall be clear on both file descriptors. (The fcntl() function can
  be used to set both these flags.)

  #include <unistd.h>

  @fds[2]: A read on the file descriptor fildes[0] shall access data
      written to the file descriptor fildes[1] on a first-in-first-out
      basis
*/
void lothars__pipe(int fds[2])
{
	if (0 > pipe(fds)) {
		err_sys("%s() error", __func__);
	}
}


/*
  sigaddset() and sigdelset() add and delete respectively signal
  signum from set.

  #ifdef _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
  #include <signal.h>

  @set: The set of signals.
  @signo: the signum to test if it is a member of set.
*/
void lothars__sigaddset(sigset_t *set, int signo)
{
	if (-1 == sigaddset(set, signo)) {
		err_sys("%s() error", __func__);
	}
}


/*
  sigaddset() and sigdelset() add and delete respectively signal
  signum from set.

  #ifdef _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
  #include <signal.h>

  @set: The set of signals.
  @signo: the signum to test if it is a member of set.
*/
void lothars__sigdelset(sigset_t *set, int signo)
{
	if (-1 == sigdelset(set, signo)) {
		err_sys("%s() error", __func__);
	}
}


/*
  sigemptyset() initializes the signal set given by set to empty, with
  all signals excluded from the set.

  #ifdef _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
  #include <signal.h>

  @set: The set of signals.
*/
void lothars__sigemptyset(sigset_t *set)
{
	if (-1 == sigemptyset(set)) {
		err_sys("%s() error", __func__);
	}
}


/*
  sigfillset() initializes set to full, including all signals.

  #ifdef _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
  #include <signal.h>

  @set: The set of signals.
*/
void lothars__sigfillset(sigset_t *set)
{
	if (-1 == sigfillset(set)) {
		err_sys("%s() error", __func__);
	}
}


/*
  sigismember() tests whether signum is a member of set.

  #ifdef _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
  #include <signal.h>

  @set: The set of signals.
  @signo: the signum to test if it is a member of set.
*/
int lothars__sigismember(const sigset_t *set, int signo)
{
	int  res;
	if (-1 == (res = sigismember(set, signo))) {
		err_sys("%s() error", __func__);
	}
	return res;
}


/*
  signal_intr / sigaction - examine and change a signal action

  The sigaction() function allows the calling process to examine
  and/or specify the action to be associated with a specific
  signal. The argument sig specifies the signal; acceptable values are
  defined in <signal.h>.

  #include <signal.h>

  @signo: The signal number.
  @func: The signal handler function.
*/
Sigfunc* signal_intr(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0; // in case adjust

	if (0 > sigaction(signo, &act, &oact)) {
		return SIG_ERR;
	}
	return oact.sa_handler;
}
Sigfunc* lothars__signal_intr(int signo, Sigfunc *func)
{
	Sigfunc *sigfunc;
	if (SIG_ERR == (sigfunc = signal_intr(signo, func))) {
		err_sys("%s() error", __func__);
	}
	return sigfunc;
}


/*
// Linux provides signal() - reimplementation
Sigfunc* signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if(signo == SIGALRM){
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
#endif
	}else{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;  // SVR4, 44BSD
#endif
	}

	if (0 > sigaction(signo, &act, &oact)) {
		return SIG_ERR;
	}
	return oact.sa_handler;
}
*/


/*
  DEPRECATED: use sigaction(), resp. lothars__signal_intr()

  The behavior of signal() varies across UNIX versions, and has also
  varied historically across different versions of Linux. Avoid its
  use: use sigaction(2) instead. See Portability below.

  signal() sets the disposition of the signal signum to handler, which
  is either SIG_IGN, SIG_DFL, or the address of a programmer-defined
  function (a "signal handler").

  #include <signal.h>

  @signo: The signal number.
  @func: The handler function to be called.
*/
Sigfunc* lothars__signal(int signo, Sigfunc *func) // for our signal() function
{
	Sigfunc *sigfunc=NULL;
	if (SIG_ERR == (sigfunc = signal(signo, func))) {
		err_sys("%s() error", __func__);
	}
	return sigfunc;
}


/*
  The sigpending() function shall store, in the location referenced by
  the set argument, the set of signals that are blocked from delivery
  to the calling thread and that are pending on the process or the
  calling thread.

  #include <signal.h>

  @set: The set of signals that are blocked from delivery.
*/
void lothars__sigpending(sigset_t *set)
{
	if (-1 == sigpending(set)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The pthread_sigmask() function shall examine or change (or both) the
  calling thread's signal mask, regardless of the number of threads in
  the process. The function shall be equivalent to sigprocmask(),
  without the restriction that the call be made in a single-threaded
  process.

  #include <signal.h>

  @how: Indicates the way in which the set is changed (SIG_BLOCK,
      SIG_SETMASK, SIG_UNBLOCK)
  @set: Points to a set of signals to be used to change the currently
      blocked set, or NULL.
  @oset: The previous mask shall be stored in the location pointed to
      by oset, or NULL.
*/
void lothars__sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	if (-1 == sigprocmask(how, set, oset)) {
		err_sys("%s() error", __func__);
	}
}


/*
  The strdup() function returns a pointer to a new string which is a
  duplicate of the string s. Memory for the new string is obtained
  with malloc(3), and can be freed with free(3).

  NB: Memory for the new string is obtained with malloc(3), and can be
  freed with free(3).

  #ifdef _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500 ||_XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
  // Since glibc 2.12: || _POSIX_C_SOURCE >= 200809L
  #include <string.h>

  @str: The string to duplicate, in case duplicates allocated memory
      by new allocation!
*/
char* lothars__strdup(const char *str)
{
	char *ptr = NULL;
	if (NULL == (ptr = strdup(str))) {
		err_sys("%s() error", __func__);
	}
	return ptr;
}


/*
  sysconf - get configuration information at run time.

  #include <unistd.h>
  #include <limits.h>

  @name: The name of the variable used to inquire about its value.
*/
long lothars__sysconf(int name)
{
	long val;
	errno = 0;  /* in case sysconf() does not change this */
	if (-1 == (val = sysconf(name))) {
		err_sys("%s() error", __func__);
	}
	return val;
}


/*
  The _sysctl() call reads and/or writes kernel parameters. For
  example, the hostname, or the maximum number of open files.

  This system call no longer exists on current kernels!

  #include <unistd.h>
  #include <linux/sysctl.h>

void lothars__sysctl(int *name, uint32_t namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	if (-1 == sysctl(name, namelen, oldp, oldlenp, newp, newlen)) {
		err_sys("sysctl error");
	}
}
// */


/*
  unlink - remove a directory entry.

  The unlink() function shall remove a link to a file. If path names a
  symbolic link, unlink() shall remove the symbolic link named by path
  and shall not affect any file or directory named by the contents of
  the symbolic link. Otherwise, unlink() shall remove the link named
  by the pathname pointed to by pathname and shall decrement the link
  count of the file referenced by the link.

  #include <unistd.h>

  @pathname: The path to be removed.
*/
void lothars__unlink(const char *pathname)
{
	if (-1 == unlink(pathname)) {
		err_sys("%s() erro for %s", __func__, pathname);
	}
}


/*
  The wait() and waitpid() functions shall obtain status information
  pertaining to one of the caller's child processes. Various options
  permit status information to be obtained for child processes that
  have terminated or stopped. If status information is available for
  two or more child processes, the order in which their status is
  reported is unspecified.

  #include <sys/wait.h>

  @iptr: If the value of the argument iptr is not a null pointer,
      information shall be stored in the location pointed to by iptr.
*/
pid_t lothars__wait(int *iptr)
{
	pid_t pid;
	if (-1 == (pid = wait(iptr))) {
		err_sys("%s() error", __func__);
	}
	return pid;
}


/*
  The wait() and waitpid() functions shall obtain status information
  pertaining to one of the caller's child processes. Various options
  permit status information to be obtained for child processes that
  have terminated or stopped. If status information is available for
  two or more child processes, the order in which their status is
  reported is unspecified.

  #include <sys/wait.h>

  @pid: The pid argument specifies a set of child processes for which
      status is requested.
  @iptr: if the value of the argument iptr is not a null pointer,
      information shall be stored in the location pointed to by iptr.
  @options: The options argument is constructed from the
      bitwise-inclusive OR of zero or more of the following flags,
      defined in the <sys/wait.h> header (see manpages).
*/
pid_t lothars__waitpid(pid_t pid, int *iptr, int options)
{
	pid_t retpid;
	if (-1 == (retpid = waitpid(pid, iptr, options))) {
		err_sys("%s() error", __func__);
	}
	return retpid;
}


