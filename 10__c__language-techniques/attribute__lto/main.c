/*
  attribute usage

  the example shows how to provide link time optimization of a
  specific version (more relevant for libraries)

  @references:
  https://gcc.gnu.org/wiki/LinkTimeOptimization

  starting gcc-v10 the -flto will be available and the corresponding
  __attribute__ declaration

   $ gcc -flto -c foo.c
   $ gcc -flto -c main.c
   $ gcc -flto -o foo.exe foo.o main.o
 or
   $ gcc -flto -o foo.exe foo.c main.c

  @author: Lothar Rubusch <l.rubusch@gmail.com>
  @license: GPLv3 2021
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "foo.h"

int main()
{
	char func[] = "main";

	fprintf(stdout, "%s():\tcall public funcion\n", func);
	if (!public_function())
		fprintf(stderr, "%s():\tpublic function failed\n", func);

	fprintf(stdout, "%s():\tcall public static function\n", func);
	if (!public_inlined_function())
		fprintf(stderr, "%s():\tpublic static function failed\n", func);

	puts("");
	puts("READY.");
	exit(EXIT_SUCCESS);
}
