// filemove.c
/*
  moves a file
//*/

#include <sys/stat.h> /* file_copy() */
#include <fcntl.h> /* file_copy() */
#include <unistd.h> /* file_copy() - low level ssize_t, read(), write(),... */

#include <time.h> /* random number generation */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset() */



/*
// the classic unix approach
//
// NOTE: rename() works for renaming and/or moving files
// anyway it cannot move cross-device, i.e. it then will
// throw cross-device link errors (errno).
//
// This is the case when e.g. moving to a mounted folder
// e.g. from a docker container to export build artifacts
int file_move(const char old_filename[FILENAME_MAX], const char new_filename[FILENAME_MAX])
{
  return rename(old_filename, new_filename);
}

/*/
// the hand made solution
//
// this solution has no problems with cross-device links
// though needs to be tested thoroughly since it is self implemented
// TODO wtf, avoid THIS?
#ifndef BUF_SIZE
#define BUF_SIZE 4096
#endif

int file_move(const char* src, const char* dst)
{
    int fd_input, fd_output;
    mode_t perm_file;
    char buf[BUF_SIZE];
    ssize_t num;

    fd_input = open(src, O_RDONLY);
    if (-1 == fd_input)
        return 1;

    perm_file = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    fd_output = open(dst, O_CREAT | O_TRUNC | O_WRONLY, perm_file);
    if (-1 == fd_output)
        return 1;

    while (0 < (num = read(fd_input, buf, BUF_SIZE)))
        if (num != write(fd_output, buf, num))
            return 1;

    if (-1 == num)
        return 1;
    if (-1 == close(fd_input))
        return 1;
    if (-1 == close(fd_output))
        return 1;

    remove(src);

    return 0;
}
// */

int main(int argc, char **argv)
{
  char file[FILENAME_MAX];
  memset(file, '\0', FILENAME_MAX);

  readstring(file, FILENAME_MAX, "open file: ");

  if (file_move(file, "Mickey-Mouse.txt")) {
    return EXIT_FAILURE;
  }

  printf("READY.\n");

  return EXIT_SUCCESS;
}
