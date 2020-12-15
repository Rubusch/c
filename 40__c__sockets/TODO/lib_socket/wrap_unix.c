//  wrap_unix.c
/*
  Socket wrapper functions.
  These could all go into separate files, so only the ones needed cause
  the corresponding function to be added to the executable.  If sockets
  are a library (SVR4) this might make a difference (?), but if sockets
  are in the kernel (BSD) it doesn't matter.
 
  These wrapper functions also use the same prototypes as POSIX.1g,
  which might differ from many implementations (i.e., POSIX.1g specifies
  the fourth argument to getsockopt() as "void *", not "char *").
 
  If your system's headers are not correct [i.e., the Solaris 2.5
  <sys/socket.h> omits the "const" from the second argument to both
  bind() and connect()], you'll get warnings of the form:
  
  warning: passing arg 2 of `bind' discards `const' from pointer target type
  warning: passing arg 2 of `connect' discards `const' from pointer target type
*/

#include "lib_socket.h"


void* _calloc(size_t n, size_t size)
{
 void *ptr = NULL;

 if(NULL == (ptr = calloc(n, size))){
   err_sys("calloc error");
 }
 return(ptr);
}



void _close(int fd)
{
  if(-1 == close(fd)){
    err_sys("close error");
  }
}



void _dup2(int fd1, int fd2)
{
  if(-1 == dup2(fd1, fd2)){
    err_sys("dup2 error");
  }
}



int _fcntl(int fd, int cmd, int arg)
{
  int res;
  
  if(-1 == (res = fcntl(fd, cmd, arg))){
    err_sys("fcntl error");
  }
  return res;
}



void _gettimeofday(struct timeval *tv, void *foo)
{
  if(-1 == gettimeofday(tv, foo)){
    err_sys("gettimeofday error");
  }
  return;
}



int _ioctl(int fd, int request, void *arg)
{
  int  res;
  
  if(-1 == (res = ioctl(fd, request, arg))){
    err_sys("ioctl error");
  }
  return(res); // streamio of I_LIST returns value
}



pid_t _fork(void)
{
  pid_t pid;
  
  if(-1 == (pid = fork())){
    err_sys("fork error");
  }
  return(pid);
}



void* _malloc(size_t size)
{
  void *ptr;
  
  if(NULL == (ptr = malloc(size))){
    err_sys("malloc error");
  }
  return(ptr);
}



int _mkstemp(char *template)
{
  int idx;
  
#ifdef HAVE_MKSTEMP
  if(0 > (idx = mkstemp(template))){
    err_quit("mkstemp error, returned -1");
  }
#else
  if((mktemp(template) == NULL) || (template[0] == 0)){
    err_quit("mktemp error, NULL or not defined");
  }

  idx = Open(template, O_CREAT | O_WRONLY, FILE_MODE);
#endif
  
  return idx;
}



#include <sys/mman.h>

void* _mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
  void *ptr;
  
  if(((void *) -1) == (ptr = mmap(addr, len, prot, flags, fd, offset))){
    err_sys("mmap error");
  }
  return ptr;
}



int _open(const char *pathname, int oflag, mode_t mode)
{
  int  fd;
  
  if(-1 == (fd = open(pathname, oflag, mode))){
    err_sys("open error for %s", pathname);
  }
  return(fd);
}



void _pipe(int *fds)
{
  if(0 > pipe(fds)){
    err_sys("pipe error");
  }
}



ssize_t _read(int fd, void *ptr, size_t nbytes)
{
  ssize_t  bytes;
  
  if(-1 == (bytes = read(fd, ptr, nbytes))){
    err_sys("read error");
  }
  return(bytes);
}



void _sigaddset(sigset_t *set, int signo)
{
  if(-1 == sigaddset(set, signo)){
    err_sys("sigaddset error");
  }
}



void _sigdelset(sigset_t *set, int signo)
{
  if(-1 == sigdelset(set, signo)){
    err_sys("sigdelset error");
  }
}



void _sigemptyset(sigset_t *set)
{
  if(-1 == sigemptyset(set)){
    err_sys("sigemptyset error");
  }
}



void _sigfillset(sigset_t *set)
{
  if(-1 == sigfillset(set)){
    err_sys("sigfillset error");
  }
}



int _sigismember(const sigset_t *set, int signo)
{
  int  res;
  
  if(-1 == (res = sigismember(set, signo))){
    err_sys("sigismember error");
  }
  return(res);
}



void _sigpending(sigset_t *set)
{
  if(-1 == sigpending(set)){
    err_sys("sigpending error");
  }
}



void _sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
  if(-1 == sigprocmask(how, set, oset)){
    err_sys("sigprocmask error");
  }
}



char* _strdup(const char *str)
{
  char *ptr = NULL;
  
  if(NULL == (ptr = strdup(str))){
    err_sys("strdup error");
  }
  return(ptr);
}



long _sysconf(int name)
{
  long val;
  
  errno = 0;  /* in case sysconf() does not change this */
  if(-1 == (val = sysconf(name))){
    err_sys("sysconf error");
  }
  return(val);
}



#ifdef HAVE_SYS_SYSCTL_H
void _sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
  if(-1 == sysctl(name, namelen, oldp, oldlenp, newp, newlen)){
    err_sys("sysctl error");
  }
}
#endif



void _unlink(const char *pathname)
{
  if(-1 == unlink(pathname)){
    err_sys("unlink error for %s", pathname);
  }
}



pid_t _wait(int *iptr)
{
  pid_t pid;
  
  if(-1 == (pid = wait(iptr))){
    err_sys("wait error");
  }
  return(pid);
}



pid_t _waitpid(pid_t pid, int *iptr, int options)
{
  pid_t retpid;
  
  if(-1 == (retpid = waitpid(pid, iptr, options))){
    err_sys("waitpid error");
  }
  return(retpid);
}



void _write(int fd, void *ptr, size_t nbytes)
{
  if(nbytes != write(fd, ptr, nbytes)){
    err_sys("write error");
  }
}
