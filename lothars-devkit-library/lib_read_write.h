#ifndef DEVKIT_LIB_READ_WRITE
#define DEVKIT_LIB_READ_WRITE


/*
  read / write functions

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/

ssize_t lothars__read(int, void *, size_t);
ssize_t lothars__readline_fd(int, void *, size_t);
ssize_t lothars__readn(int, void *, size_t);
ssize_t lothars__read_fd(int, void*, size_t, int *);

void lothars__write(int, void *, size_t);
void lothars__writen(int, void *, size_t);
ssize_t lothars__write_fd(int, void*, size_t, int);


#endif /* DEVKIT_LIB_READ_WRITE */
