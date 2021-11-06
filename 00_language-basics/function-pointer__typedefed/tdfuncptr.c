// tdfuncptr.c
/*
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// some function
void some_function()
{
	puts("READY.");
}

int main()
{
	// definition
	typedef void (*funptr)();
	funptr p_some_function;

	// init
	p_some_function = &some_function;

	// usage
	p_some_function();

	return 0;
}
