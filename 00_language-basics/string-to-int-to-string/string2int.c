/*
 * @author: Lothar Rubusch
 * @date: 2013-May-24
 *
 * string to int and back int to string
 */

#include <stdio.h>
#include <stdlib.h> // atoi()
#include <string.h>

int main(int argc, char **argv)
{
	int val = 123;
	int anotherval = 0;
	char str[32];
	memset(str, '\0', 32);

	// int to string: sprintf()
	sprintf(str, "%d", val);
	printf("char str '%s'\n", str);

	// string to int: atoi()
	anotherval = atoi(str);
	printf("int value '%d'\n", anotherval);

	puts("READY.");
	return EXIT_SUCCESS;
}
