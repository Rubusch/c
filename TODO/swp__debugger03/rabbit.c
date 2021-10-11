#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TRUE 1
/* just some demo code */
int main(int argc, char **argv)
{
	int cnt = 0;
	while (TRUE) {
		printf("RABBIT: '%d'\n", cnt);
		cnt++;
		sleep(1);
	}
	exit(EXIT_SUCCESS);
}
