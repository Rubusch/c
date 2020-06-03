/*
  foo.c
*/

#include "foo.h"

/*
  the variable 'private' and functions using 'private' here
  in the .c cannot be exported as function, only as macro!
 */
static int private = 777;

int public_function()
{
	char func[] = "public_function";

	fprintf(stdout, "%s():\ta public function is declared in .h and defined in .c file\n", func);

	fprintf(stdout, "%s():\ta 'static variable in .c file' is a private member to this compile unit: '%d'\n", func, private);

	fprintf(stdout, "%s():\tcall to public static function\n", func);

	public_inlined_function();

	puts("");
	return 0;
}
