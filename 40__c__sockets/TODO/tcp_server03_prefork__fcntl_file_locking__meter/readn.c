// readn.c

#include <stddef.h> // size_t
#include <stdint.h> // SIZE_MAX
#include <errno.h> // errno
#include <stdio.h> // frpintf()
#include <unistd.h> // read()


size_t readn( int fd, void *vptr, size_t siz )
{
        size_t nleft = --siz; // we omit the '\0'
        size_t nread = -1;
        char *ptr = vptr;

        while (0 < nleft) {
                // looping over read(fd,...) and just picking out 'nleft'
                // characters results in first picking out 'nleft' characters
                // and in the next turn picking out 0 characters, thus break,
                // thus only the first 'nleft' characters will be read here, at
                // most SSIZE_MAX for POSIX and ssize_t (i.e. something like
                // 2147483647, or SIZE_MAX, i.e. 65k for regular C
                // read reads up to SSIZE_MAX characters
                //
                // nleft must be lesser than SIZE_MAX for read
                if (0 > (nread = read( fd, ptr, nleft%SIZE_MAX ))) {
                        if (errno == EINTR) {
                                nread = 0;
                        } else {
                                return -1;
                        }

                } else if (0 == nread) {
                        fprintf( stderr, "read nothing\n" );
                        break;
                }
                nleft -= nread;
                ptr += nread;
                fprintf( stderr, "\t%s:%d - '%s' received, '%d' characters " \
                         "read, '%d' fields in BUF could still be read\n"
                         , __FILE__
                         , __LINE__
                         , (char*) vptr
                         , nread
                         , nleft);
                break; // read less than siz characters
        }
        return (siz - nleft);
}
