// getch.h
/*
  Doesn't work with gcc on cygwin!
//*/

#ifndef GETCH_PORTABLE
#define GETCH_PORTABLE

#include <stdio.h>
#include <stdlib.h>

#endif

#ifdef __unix__
#include <termios.h>
#include <unistd.h>

static struct termios new_io;
static struct termios old_io;
/*
  set terminal in cbreak mode
  set ECHO and ICANON to 0
  control options: reading brings 1 Byte VMIN=1 VTIME=1
//*/

int cbreak(int fd){
  // save terminal
  if((tcgetattr(fd, &old_io)) == -1) return -1;
  new_io = old_io;

  // define cbreak mode
  new_io.c_lflag = new_io.c_lflag & ~(ECHO|ICANON);
  new_io.c_cc[VMIN] = 1;
  new_io.c_cc[VTIME] = 0;

  // set cbreak mode
  if((tcsetattr(fd, TCSAFLUSH, &new_io)) == -1)
    return -1;
  return 1;
}

int getch(void)
{
  int c;
  if(cbreak(STDIN_FILENO) == -1){
    printf("cbreak funkction failed\n");
    exit(EXIT_FAILURE);
  }
  c = getchar();

  // reset old terminal mode
  tcsetattr(STDIN_FILENO, TCSANOW, &old_io);
  return c;
}

#elif __WIN32__ || _MSC_VER || __MS_DOS__
#include <conio.h>
#endif


/* Demo */
#define DEMOCODE
//*/
#ifdef DEMOCOE
int main(char** argv, int argc)
{
  int sz;
  printf("Enter \'q\' to terminate the prg.\n");

  while((sz=getch()) != 'q')
    ;

  return EXIT_SUCCESS;
};
#endif
