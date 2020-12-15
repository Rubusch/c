// writen.c

#include <stddef.h> // size_t
#include <errno.h> // errno
#include <unistd.h> // write()


size_t writen( int fd, const void *vptr, size_t len )
{
        size_t nleft = len;
        size_t nwritten = -1;
        const char *ptr = vptr;

        while (0 < nleft) {
                if (0 >= (nwritten = write( fd, ptr, nleft))) {
                        if (nwritten < 0 && errno == EINTR) {
                                nwritten = 0;
                        } else {
                                return -1;
                        }
                }
                nleft -= nwritten;
                ptr += nwritten;
        }
        return len;
}
