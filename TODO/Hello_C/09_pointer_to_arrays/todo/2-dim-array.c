/*
  2-dim-array
 
  This is kind of the most common structure, a pointer to arrays or an array of
a pointer. here is just one as example: a pointer to arrays.

  Conclusion:

  char** ptr; // has a first pointer (unsigned long int because of the address
size!) and a second pointer to a value of the type itself, here a 'char'.

  Nowadays the size of addresses is 64 bit or unsigned long int!

  &ptr;   // the address of the first pointer itself (unsigned long int)
  ptr;    // the address of the second pointer (unsigned long int)
  *ptr;   // the content of the first pointer, the address of the char value
(unsigned long int)
  **ptr;  // the content of the second pointer: a char value (8 bit)

output:

Size of the array is: 4 - FAILS due to the pointer!

Size of the array is: 4 - FAILS due to pass!
0. element:	's', 't', 'r', ' ', '0', ' '
1. element:	's', 't', 'r', ' ', '1', ' '
2. element:	's', 't', 'r', ' ', '2', ' '
3. element:	's', 't', 'r', ' ', '3', ' '
4. element:	's', 't', 'r', ' ', '4', ' '
5. element:	's', 't', 'r', ' ', '5', ' '
6. element:	's', 't', 'r', ' ', '6', ' '
7. element:	's', 't', 'r', ' ', '7', ' '
8. element:	's', 't', 'r', ' ', '8', ' '
9. element:	's', 't', 'r', ' ', '9', ' '

READY.
//*/

// given inits
// TODO

#define ELEMENT_COUNT 10
#define ELEMENT_LENGTH 8

// forwared declaration of a function with 2-dim-array
// TODO

int main(int argc, char **argv)
{
	// Definition of a pointer to arrays - Attention: use parenthesis!!!
	char(*arr)[ELEMENT_LENGTH];

	// Allocation of a pointer to arrays
	// TODO

	// initialization of the pointer to arrays with "str x", x is a number,
	// starting by 0
	char str[6];
	int cnt;
	for (cnt = 0; cnt < ELEMENT_COUNT; ++cnt) {
		// TODO
	}

	// size of a pointer to arrays
	printf("Size of the array is: %i - FAILS due to the pointer!\n",
	       (sizeof arr));

	// Pass and call of a function with pointer to array
	output(arr);

	// free
	// TODO

	puts("READY.");
	return 0;
}

// Definition of a funcion with pointer to arrays
void output(char (*arr)[ELEMENT_LENGTH])
{
	// CAUTION: here the size of the array is unknown - has to be passed as
	// param!!!
	printf("\nSize of the array is: %i - FAILS due to pass!\n",
	       (sizeof arr));

	// print out
	int idx;
	int jdx;
	// TODO
	printf("\n");
}
