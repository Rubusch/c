// main.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * tester for compilation
//*/

#include "console_input.h"

int main(int argc, char **argv)
{
	unsigned int foo = 0;
	readnumber(&foo, 5, "enter a value");

	exit(EXIT_SUCCESS);
}
