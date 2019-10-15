// getch.h
/*
  Doesn't work with gcc on cygwin!
//*/

#include <stdio.h>
#include <stdlib.h>

#include <termios.h> /* tcsetattr() */
#include <unistd.h>  /* tcsetattr() */

static struct termios new_io;
static struct termios old_io;
//  set terminal in cbreak mode
//  set ECHO and ICANON to 0
//  control options: reading brings 1 Byte VMIN=1 VTIME=1

int cbreak(int fd)
{
  // save terminal
  if (-1 == (tcgetattr(fd, &old_io))) {
    return -1;
  }
  new_io = old_io;

  // define cbreak mode
  new_io.c_lflag = new_io.c_lflag & ~(ECHO | ICANON);
  new_io.c_cc[VMIN] = 1;
  new_io.c_cc[VTIME] = 0;

  // set cbreak mode
  if (-1 == (tcsetattr(fd, TCSAFLUSH, &new_io))) {
    return -1;
  }
  return 1;
}

int getch(void)
{
  int c;
  if (-1 == cbreak(STDIN_FILENO)) {
    printf("cbreak function failed\n");
    exit(EXIT_FAILURE);
  }
  c = getchar();

  // reset (very) old terminal mode
  tcsetattr(STDIN_FILENO, TCSANOW, &old_io);
  return c;
}


/* Demo */
int main(int argc, char **argv)
{
  int sz;
  printf("Enter \'q\' to terminate the prg.\n");

  while ((sz = getch()) != 'q') {
    // do some print out of the read characters
    puts(( char * )&sz);
  }

  return EXIT_SUCCESS;
}
