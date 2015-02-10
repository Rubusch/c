// address.c
/*
  addresses and set of variables 

  a variable can have a value a pointer points to a place in memory which is another address
  definitions:
  lvalue      in an assignment the left (l) value, means the value to be assign to, e.g.:
              int lvalue = 7;
	      the variable lvalue here is a "l-value"

  reference:  a '&' indicates a reference, a reference is the address in memory or "another name to a variable, 
              it's not a pointer, it's the variable itself" - this is the definition like it is writen in books. 
	      I never digged much into that, since I think this is very cryptic. The essence of this is that 
	      with a '&' you gain the address in memory of a variable it self.  
	      Hence a &* e.g. in &*ptr is the address of the thing the pointer "ptr" points to, for simplicity 
	      in C this is the same as directly writing: ptr

  pointer:    a '*' marks a pointer, a pointer has an address itself, e.g.: &ptr
	      it points to an address, e.g.: ptr or &*ptr (which is the same)
	      and can be "dereferenced", e.g.: *ptr 
	      The dereferenced pointer is the content which is stored at the address where the pointer points to.

  dereferenced: 
              the C Processor (Compiler) distinguishes between two types of variables: "references" and "types"
	      The principal diference is a reference can't be a lvalue, a type can!
	      Types are variables of int, char, double, long, etc... references are normally displayed normally 
	      as hex values or unsigned long decimals. 
	      A pointer e.g. char *ptr, by default diplays the reference (address) of the content it points to:
	      ptr (which is the same as &*ptr)
	      to get the content, in this case the char(-acter) which is stored on this address, the pointer needs 
	      to be de-referenced, this happenes by placing an asterisk in front:
	      *ptr 
	      This displayes the content stored on the address referenced to by the pointer 
	      (= the de-referenced pointer!)


  Output:

  $ ./address.exe
  addresses
  allocation and init
  set values
  address of values:
       char_value      - 0x22cce3
       int_value       - 0x22cce4

  address of pointers:
       char_pointer    - 0x22ccd8
       int_pointer     - 0x22ccdc

  address of content of pointers:
       *char_pointer   - 0x6701a8
       *int_pointer    - 0x670178

  content of pointers (the current element!)
       *char_pointer   - 7
       *int_pointer    - 7

  READY.
//*/

// TODO: include libraries

// size definitions
#define INT_SIZE 10
#define CHAR_SIZE 10


int main()
{
  puts("addresses");

  // definition of variables
  int int_value = 0;
  char char_value = 0;
  int* int_pointer = NULL;
  char* char_pointer = NULL;

  puts("allocation and init");

  // alloc int_pointer
  // TODO


  // init and clean the content of int_pointer
  // 0 is a number
  memset(int_pointer, 0, sizeof(*int_pointer) * INT_SIZE);


  // alloc char_pointer
  // TODO


  // init and clean the content of char_pointer
  // 1. init a string: better to initialize each element as "end of string" == \0
  // 2. \0 is a character: 
  //     single characters have ' e.g.: 'a', 'b', '1', '2', or '\n'
  //     everything with a " is a string and has one more token, the '\0'
  //     e.g.: "a" == {'a', '\0'} or "abc" == {'a', 'b', 'c', '\0'}
  memset(char_pointer, '\0', CHAR_SIZE);


  puts("set values");
  // set int_value to 7
  int_value = 7;

  // set char_value to 7 (as character token)
  // TODO

  // set each element of the int array to 7 
  // TODO (you will need a for loop, name the index: idx!)

  // set each element of the char array to 7
  // TODO
 
  
  // print out address of..
  // ... values
  puts("address of values:");
  printf("\tchar_value\t- 0x%lx\n", (unsigned long) &char_value);
  // TODO
  puts("");


  // ... content address of pointers
  puts("address of pointers:");
  // TODO


  // ... content address of pointers
  puts("address of content of pointers:");
  // TODO
  printf("\t*int_pointer\t- 0x%lx\n", (unsigned long) &*int_pointer);
  puts("");


  // ... content of pointers
  puts("content of pointers (the current element!)");
  printf("\t*char_pointer\t- %c\n", *char_pointer);
  printf("\t*int_pointer\t- %d\n", *int_pointer);
  puts("");


  // What is missing here?
  // TODO

  puts("READY.");
  exit(EXIT_SUCCESS);
}
