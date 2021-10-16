#include <stdlib.h>
#include <stdio.h>

#include "utility.h"

int main(void)
{
	int x = 1;
	int y = 3;
	fprintf(stdout, "my_add(%d, %d):\t%d\n", x, y, my_add(x, y));
	fprintf(stdout, "my_multiply(%d, %d):\t%d\n", x, y, my_multiply(x, y));
	exit(EXIT_SUCCESS);
}
