/*
  Compound Literal Operator (C99)


  Syntax:
  ( type ) { initializer-list }

  e.g.
  (const char []){"abc"}; // might be 1 or 0, implementation-defined


  type - a type name specifying any complete object type or an array
      of unknown size, but not a variable-length array (VLA)

  initializer-list - list of initializers suitable for initialization
      of an object of type


  The compound literal expression constructs an unnamed object of the
  type specified by type and initializes it as specified by
  initializer-list. Designated initializers are accepted.

  The type of the compound literal is type (except when type is an
  array of unknown size; its size is deduced from the initializer-list
  as in array initialization).

  The value category of a compound literal is lvalue (its address can
  be taken).

  The unnamed object to which the compound literal evaluates has
  static storage duration if the compound literal occurs at file scope
  and automatic storage duration if the compound literal occurs at
  block scope (in which case the object's lifetime ends at the end of
  the enclosing block).


  References:
  https://en.cppreference.com/w/c/language/compound_literal
*/

#include <stdlib.h>
#include <stdio.h>

#include <math.h> /* pow() */

// Each compound literal creates only a single object in its scope:
int func (void)
{
	struct strunk {int mem;} *ptr = 0, *quack;
	int idx = 0;

	/*
	  Because compound literals are unnamed, a compound literal
	  cannot reference itself (a named struct can include a
	  pointer to itself)

	  Although the syntax of a compound literal is similar to a
	  cast, the important distinction is that a cast is a
	  non-lvalue expression while a compound literal is an lvalue.
	 */

again:
	/* compound literal: scoped usage */
	quack = ptr, ptr = &((struct strunk){ idx++ }); // compound literal
	if (idx < 2) {
		printf("\t%d. ptr->mem = %d\n", idx, ptr->mem);
		goto again;
	}
	/*
	  note: if a loop were used, it would end scope here, which
	  would terminate the lifetime of the compound literal leaving
	  p as a dangling pointer!!!
	*/
	return ptr == quack && quack->mem == 1; // always returns 1
}


/* compound literal: global usage */
int *pglobal = (int[]) {2, 4};
    // creates an unnamed static array of type int[2]
    // initializes the array to the values {2, 4}
    // creates pointer p to point at the first element of the array

/* compound literal: read-only compound literal */
const float *preadonly = (const float []) {1e0, 1e1, 1e2};


/* compound literal: TODO */
struct point {double x,y;};
void drawline_by_value(struct point from, struct point to)
{
	printf("\t%s()\n\tfrom.x == %f, from.y == %f and to.x == %f, to.y == %f\n\n", __func__, from.x, from.y, to.x, to.y);
}

void drawline_by_address(struct point *from, struct point *to)
{
	printf("\t%s()\n\tfrom->x == %f, from->y == %f and to->x == %f, to->y == %f\n\n", __func__, from->x, from->y, to->x, to->y);
}



int main(void)
{
	printf("C99: compound literal and string literals\n");
	/*
	  Compound literals of const-qualified character or wide
	  character array types may share storage with string
	  literals.

	  With gcc evaluates to "undefined behavior" and fails
	  compilation.
	*/
//	printf("evaluates '%s'\n", ((const char []){"abc"} == "abc") ? "1" : "0");
	printf("\t...does not compile (gcc)\n");


	printf("\nC99: compound literal and scope\n");
	func();

	printf("\nC99: global and readonly\n");
	for (int idx=0; idx<2; idx++)
		printf("\tpglobal[%d] = %d [%d]\n", idx, pglobal[idx], 2*(idx+1));

	for (int idx=0; idx<3; idx++)
		printf("\tpreadonly[%d] = %f [%f]\n", idx, preadonly[idx], pow(10.0, (float)idx));


	printf("\nC99: compound literal and unnamed automatic arrays\n");
	int num = 2, *pipo = &num;
	pipo = (int [2]){*pipo};
        /* creates an unnamed automatic array of type int[2]

	   initializes the first element to the value formerly held in
	   *pipo, 2

	   initializes the second element to zero and stores the
	   address of the first element in pipo
	*/
	printf("\tpipo[0] == %d [2]\n", pipo[0]);
	printf("\tpipo[1] == 0x%X [0x0]\n", pipo[1]);

	printf("\nC99: compound literal and block scope\n");

	drawline_by_value((struct point){.x=1, .y=1}, (struct point){.x=3, .y=4});
        /* creates two  structs with block scope  and calls drawline1,
	 * passing them by value */

	drawline_by_address(&(struct point){.x=1, .y=1}, &(struct point){.x=3, .y=4});
        /* creates two structs with block scope and calls drawline2,
	 * passing their addresses */


	printf("\nREADY.\n");
	exit(EXIT_SUCCESS);
}
