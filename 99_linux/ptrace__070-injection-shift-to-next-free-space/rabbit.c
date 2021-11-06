/*
  dummy example to be debugged stepwise

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Oct 31, 2002  By Pradeep Padala
 */

#include <stdio.h> /* printf() */
#include <stdlib.h>
#include <unistd.h> /* sleep() */

int main(int argc, char **argv)
{
	int cnt;
	for (cnt = 0; cnt < 3; ++cnt) {
		printf("counter: '%d'\n", cnt);
		sleep(1);
	}
	return 0;
}
