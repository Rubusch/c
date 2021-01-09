#ifndef DEVKIT_LIB_UNIX
#define DEVKIT_LIB_UNIX

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
