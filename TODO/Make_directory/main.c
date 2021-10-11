// main.c

#include <stdio.h>
#include <stdlib.h>

#include "directory/folder.h"

extern void func(const char *);

int main(int argc, char **argv)
{
	func("Hello World!");

	exit(EXIT_SUCCESS);
}
