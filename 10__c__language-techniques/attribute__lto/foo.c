/*
  foo.c
*/

#define _GNU_SOURCE

#include "internal.h"
#include "foo.h"

/*
  the variable 'private' and functions using 'private' here
  in the .c cannot be exported as function, only as macro!
 */
static int private = 777;

IMPL_SYMVER(function, "1.0")
int new_function()
{
	char func[] = "new_function";

	fprintf(stdout,
		"%s():\ta new function is declared in .h and defined in .c file\n",
		func);

	fprintf(stdout,
		"%s():\ta 'static variable in .c file' is a private member to this compile unit: '%d'\n",
		func, private);

	fprintf(stdout, "%s():\tcall to new static function\n", func);

	public_inlined_function();

	puts("");
	return 0;
}
