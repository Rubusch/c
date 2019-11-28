/*
  show instructions of another program, e.g. of one loop iteration

  start as follows
  $ ./rabbit.exe & ./ptracer.exe $(pidof rabbit.exe)

  resource: Linux Journal, Oct 31, 2002  By Pradeep Padala

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
 */

#include <stdio.h> /* printf() */
#include <stdlib.h>
#include <unistd.h> /* sleep() */

int main(int argc, char **argv)
{
  int cnt;
  for (cnt = 0; cnt < 10; ++cnt) {
    printf("counter: '%d'\n", cnt);
    sleep(3);
  }
  return 0;
}
