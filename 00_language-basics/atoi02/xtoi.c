// xtoi.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * convert a string into int and transform the int number into a char (stored)
 * number
//*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  converts a string stored hex number into an
  integer for capitals as lower case

  returns the result or stores it in the passed unsigned pointer
//*/
int xtoi(const char *origin, unsigned int *result)
{
	size_t szlen = strlen(origin);
	int idx, ptr, factor;

	if (szlen > 0) {
		if (szlen > 8)
			return 0;
		*result = 0;
		factor = 1;

		for (idx = szlen - 1; idx >= 0; --idx) {
			if (isxdigit(*(origin + idx))) {
				if (*(origin + idx) >= 97) {
					ptr = (*(origin + idx) - 97) + 10;
				} else if (*(origin + idx) >= 65) {
					ptr = (*(origin + idx) - 65) + 10;
				} else {
					ptr = *(origin + idx) - 48;
				}
				*result += (ptr * factor);
				factor *= 16;
			} else {
				return 4;
			}
		}
	}

	return 1;
}

int main(int argc, char **argv)
{
	char source[3] = "ff";

	puts("convert a string stored number to a char stored numerical value");

	puts("1. converto to int");
	printf("\tsource: %s\n", source);

	unsigned int tmp = 0;
	xtoi(source, &tmp);
	printf("\tatoi() output: %i\n", tmp);

	puts("2. converto int to char");
	unsigned char destination = 0;
	/*
    either typedeffing
  //*/
	destination = (char)tmp;

	/*
    or memcopy:
  memcpy(&destination, &tmp, 1);
  //*/
	printf("\tresult: %i\n", destination);

	puts("READY.");
	exit(EXIT_SUCCESS);
}
