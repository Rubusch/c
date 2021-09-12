/*
  callback function

  register function pointer to be called at event - this is a
  quick&dirty syntax demo

  REFERENCES
  https://stackoverflow.com/questions/142789/what-is-a-callback-in-c-and-how-are-they-implemented

  @author: Lothar Rubusch <lothar.rubusch@gmail.com>
  @license: GPLv3
*/

#include <stdlib.h>
#include <stdio.h>

void populate_array(int *array, size_t arraySize, int (*getNextValue)(void))
{
	fprintf(stdout, "execute the passed function pointer\n");
	for (size_t i=0; i<arraySize; i++) {
		array[i] = getNextValue();
		fprintf(stdout, "array[%d] = '%d'\n", i, array[i]);
	}
}

int getNextRandomValue(void)
{
	return rand();
}

int main(void)
{
	int myarray[10];

	fprintf(stdout, "call 'register' function\n");
	populate_array(myarray, 10, getNextRandomValue);

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
