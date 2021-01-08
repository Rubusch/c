/*
  alignof (C11)


  This operator is typically used through the convenience macro
  alignof, which is provided in the header stdalign.h

  Returns the alignment requirement of the type named by type-name. If
  type-name is an array type, the result is the alignment requirement
  of the array element type. The type-name cannot be function type or
  an incomplete type.

  The result is an integer constant of type size_t.

  The operand is not evaluated (so external identifiers used in the
  operand do not have to be defined).

  If type-name is a VLA type, its size expression is not evaluated.

  max_align_t is a type whose alignment requirement is at least as
  strict (as large) as that of every scalar type.


  Usage (Unix/Linux):
  $ make
  $ ./alignof.exe


  References:
  https://en.cppreference.com/w/c/language/_Alignof
*/
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <inttypes.h> /* C11: fixed int types - PRIxPTR */

#include <stdalign.h> /* C11: alignof */


int main(void)
{
	printf("\nC11: type max_align_t\n");
	size_t a = alignof(max_align_t);
	printf("\tAlignment of max_align_t is %zu (%#zx)\n", a, a);

	void *p = malloc(123);
	printf("\tThe address obtained from malloc(123) is %#" PRIxPTR"\n", (uintptr_t)p);
	free(p);


	printf("\nC11: operator alignof()\n");
	printf("\tAlignment of char = %zu\n", alignof(char));
	printf("\tAlignment of max_align_t = %zu\n", alignof(max_align_t));
	printf("\t\talignof(float[10]) = %zu\n", alignof(float[10]));

	printf("\t\talignof(struct{char c; int n;}) = %zu\n", alignof(struct {char c; int n;}));


	printf("\nREADY.\n");
	exit(EXIT_SUCCESS);
}
