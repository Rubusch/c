/*
  2-dim-array

  a real 2 dimensional array is an array (statical) of an array (statical)! It's
a statical data structure and every array refered by in the first array has the
SAME size!

  number of elements:
  {number of all emlements} == {number of elements in array A} x {number of
elements in B}

  A pointer doesn't know if it points to an array of elements or, respectively,
to more elements of allocated memory. A pointer only knows that address it
points to AND the size of the value that is stored there (=the type of the
pointer).

  Example:
  if a char* ptr get's initialized, it points to an address where one 'char' (=8
bit) are stored, therefore it's a pointer to 'char'.

  An array of an array, is - thought in pointer - a pointer to another pointer
(and both pointing to static memory, means not allocated).



output:

Size of the array is: 12, expected: 12
Number of elements in the array: 12 x 10 == 120

Size of the array is: 0 - FAILS!
0. element:	'J', 'a', 'n', 'u', 'a', 'r', 'y', ' '
1. element:	'F', 'e', 'b', 'r', 'u', 'a', 'r', 'y', ' '
2. element:	'M', 'a', 'r', 'c', 'h', ' '
3. element:	'A', 'p', 'r', 'i', 'l', ' '
4. element:	'M', 'a', 'y', ' '
5. element:	'J', 'u', 'n', 'e', ' '
6. element:	'J', 'u', 'l', 'y', ' '
7. element:	'A', 'u', 'g', 'u', 's', 't', ' '
8. element:	'S', 'e', 'p', 't', 'e', 'm', 'b', 'e', 'r', ' '
9. element:	'O', 'c', 't', 'o', 'b', 'e', 'r', ' '
10. element:	'N', 'o', 'v', 'e', 'm', 'b', 'e', 'r', ' '
11. element:	'D', 'e', 'c', 'e', 'm', 'b', 'e', 'r', ' '


As you can see the words don't fill up all the space that would be possible in
an 2-dim-array therefore we can use another structure: a pointer to arrays or to
other pointers!

READY.
//*/

// given inits
// TODO

// allways avoid "magic numbers in code" and try to define "constant" values (in
// c these are "defines")
#define ELEMENT_COUNT 12
#define ELEMENT_LENGTH 10

// Forward declaration of the output function
// Attention: the size of the second pointer has to be specified already here!
// TODO

int main(int argc, char **argv)
{
	// Definition of a 2-dim-array
	char arr[ELEMENT_COUNT][ELEMENT_LENGTH] = {
		"January",   "February", "March",    "April",
		"May",       "June",     "July",     "August",
		"September", "October",  "November", "December"
	};

	// print the size of 2-dim-array
	printf("Size of the array is: %d, expected: %d\n",
	       (sizeof(arr) / ELEMENT_LENGTH), ELEMENT_COUNT);
	printf("Number of elements in the array: %d x %d == %d\n", /* TODO */);

	// Pass and call of the function with a 2-dim-array
	// TODO

	puts("READY.");
	return 0;
}

// Definition of a funcion with 2-dim-array
void output(char arr[][ELEMENT_LENGTH])
{
	// CAUTION: here the "sizeof" of the static array is unknown - has to be
	// passed as param!!!
	printf("\nSize of the array is: %i - FAILS!\n",
	       (sizeof(arr) / ELEMENT_LENGTH));

	// print out
	int idx;
	int jdx;
	for (idx = 0; idx < ELEMENT_COUNT; ++idx) {
		printf("%i. element:\t", idx);
		for (jdx = 0; jdx < (strlen(arr[idx]) + 1); ++jdx) {
			printf("%s\'%c\'", (/* TODO */), arr[idx][jdx]);
		}
		printf("\n");
	}

	printf("\n\nAs you can see the words don't fill up all the space that would "
	       "be possible in an 2-dim-array\n");
	printf("therefore we can use another structure: a pointer to arrays or to "
	       "other pointers!\n\n");
}
