/*
  attribute usage

  the demo shows how to use the unused attribute declaration;

  without declaration:
    gcc -g -Wall -Werror -std=c11 -pedantic   -c -o Hello.o Hello.c
    Hello.c: In function ‘main’:
    Hello.c:16:6: error: unused variable ‘idx’ [-Werror=unused-variable]
      int idx;
          ^~~
    cc1: all warnings being treated as errors
    make: *** [<builtin>: Hello.o] Error 1


  @references:
  http://unixwiz.net/techtips/gnu-c-attributes.html

  @author: Lothar Rubusch <l.rubusch@gmail.com>
  @license: GPLv3 2021
 */

#include <stdio.h>
int main(int argc __attribute__((unused)),
	 char *argv[]) /* apply to function arguments */
{
	int idx __attribute__((unused)); /* apply to variable declarations */

	printf("Hello World!\n");
	return 0;
}
