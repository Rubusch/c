/*
  foo.h
*/

#ifndef FOO_H_
#define FOO_H_

#include <stdio.h>

int public_function();

static int public_inlined_function()
{
	char func[] = "public_inlined_function";
	fprintf(stdout,
		"%s():\ta public static function can be implemented in .h file (inlined)\n",
		func);
	puts("");

	return 0;
}

#endif /* FOO_H_ */
