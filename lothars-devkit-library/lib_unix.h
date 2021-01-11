#ifndef DEVKIT_LIB_UNIX
#define DEVKIT_LIB_UNIX


// includes

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h> /* va_list, va_start,... */
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <fcntl.h> /* fcntl() */
#include <stropts.h> /* ioctl() */
#include <sys/mman.h> /* mmap() */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h> /* gettimeofday() */
#include <sys/wait.h>

#include "lib_error.h"


// constants

typedef void Sigfunc(int); /* give signal handlers a type instead of void* */

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


// forwards

void* lothars__calloc(size_t, size_t);
void lothars__close(int);
void lothars__dup2(int, int);
int lothars__fcntl(int, int, int);
void lothars__gettimeofday(struct timeval *);
int lothars__ioctl(int, int, void *);
pid_t lothars__fork();
void* lothars__malloc(size_t);
int lothars__mkstemp(char *);
void* lothars__mmap(void *, size_t, int, int, int, off_t);
int lothars__open(const char *, int, mode_t);
void lothars__pipe(int *fds);
void lothars__sigaddset(sigset_t *, int);
void lothars__sigdelset(sigset_t *, int);
void lothars__sigemptyset(sigset_t *);
void lothars__sigfillset(sigset_t *);
int lothars__sigismember(const sigset_t *, int);
Sigfunc* lothars__signal(int, Sigfunc*);
Sigfunc* lothars__signal_intr(int, Sigfunc*);  // missing
void lothars__sigpending(sigset_t *);
void lothars__sigprocmask(int, const sigset_t *, sigset_t *);
char* lothars__strdup(const char *);
long lothars__sysconf(int);
void lothars__sysctl(int *, uint32_t, void *, size_t *, void *, size_t);
void lothars__unlink(const char *);
pid_t lothars__wait(int *);
pid_t lothars__waitpid(pid_t, int *, int);


#endif /* DEVKIT_LIB_UNIX */
