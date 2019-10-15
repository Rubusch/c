// length.c
/*
  A dynamic array is a pointer with some allocated space. This space of memory
is allocated in an area of memory called the "heap", memory on the heap is
called dynamic memory! Its size can be changed later on. Allocated memory ALWAYS
has to be freed or a memory leak results! a dynamic array's pointer is e.g. char
*arr;

  A static array (static has NOTHING to do with the keyword "static" in the
code!!!) has a fixed size, its memory is located in an area of memory called the
stack! The size has to be defined at beginning and can't be changed later on.
The array variable is a pointer, e.g. char arr[10];

  output:

$ ./length.exe
allocation and init

int_pointer:
sizeof(int_pointer)     = 4
elements of int_pointer = 1

char_pointer:
sizeof(char_pointer)    = 4
elements of char_pointer= 4
strlen(char_pointer)    = 9

int array:
sizeof(int_array)       = 40
elements of int_array   = 10

char array:
sizeof(char_array)      = 10
elements of char_array  = 10
strlen(char_array)      = 9

as you can see:
        1. sizeof() has NO knowledge about allocated space.
        2. sizeof() has NO knowledge about sizes of arrays in a called function
func with pointers
        function
                pInt:
                sizeof(pInt)    = 4
                elements of pInt= 1

                pCh:
                sizeof(pCh)     = 4
                elements of pCh = 4
                strlen(pCh)= 9

        function finished

func with arrays
        function
                pInt:
                sizeof(pInt)    = 4
                elements of pInt= 1

                pCh:
                sizeof(pCh)     = 4
                elements of pCh = 4
                strlen(pCh)= 9

        function finished

READY.
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_SIZ 10
#define CHAR_SIZ 10


void func(char *pCh, unsigned int siz_ch, int *pInt, unsigned int siz_i)
{
  puts("\tfunction");

  // check for NULL in pointers (a can, not a MUST!):
  if (NULL == pCh)
    return;
  if (NULL == pInt)
    return;

  // do the actual work in the function
  puts("\t\tpInt:");
  printf("\t\tsizeof(pInt)\t= %d\n", sizeof(pInt));
  printf("\t\telements of pInt= %d\n", sizeof(pInt) / sizeof(int));
  puts("");

  puts("\t\tpCh:");
  printf("\t\tsizeof(pCh)\t= %d\n", sizeof(pCh));
  printf("\t\telements of pCh\t= %d\n", sizeof(pCh) / sizeof(char));
  printf("\t\tstrlen(pCh)= %d\n", strlen(pCh));
  puts("");
  puts("\tfunction finished\n");
}


int main()
{
  // definition of variables
  int *int_pointer = NULL;
  char *char_pointer = NULL;
  int int_array[INT_SIZ];
  char char_array[CHAR_SIZ];

  puts("allocation and init");
  // alloc and clean int_pointer
  if (NULL == (int_pointer = calloc(INT_SIZ, sizeof(*int_pointer)))) {
    perror("calloc failed");
    exit(EXIT_FAILURE);
  }
  memset(int_pointer, 0, INT_SIZ);


  // alloc and clean char_pointer
  if (NULL == (char_pointer = calloc(CHAR_SIZ, sizeof(*char_pointer)))) {
    perror("calloc failed");
    exit(EXIT_FAILURE);
  }
  memset(char_pointer, '\0', CHAR_SIZ);

  // init arrays
  memset(int_array, 0, INT_SIZ);
  memset(char_array, '\0', CHAR_SIZ);


  // set each element of the int array to 7
  int idx = 0;
  for (idx = 0; idx < INT_SIZ; ++idx) {
    int_pointer[idx] = 7;
  }

  // set each element of the char array to 7
  strncpy(char_pointer, "777777777", CHAR_SIZ);

  // set each element of the int array to 7
  for (idx = 0; idx < INT_SIZ; ++idx) {
    int_array[idx] = 7;
  }

  // set each element of the char array to 7
  strncpy(char_array, "777777777", CHAR_SIZ);

  puts("");


  // do the actual work
  puts("int_pointer:");
  printf("sizeof(int_pointer)\t= %d\n", sizeof(int_pointer));
  printf("elements of int_pointer\t= %d\n", sizeof(int_pointer) / sizeof(int));
  puts("");

  puts("char_pointer:");
  printf("sizeof(char_pointer)\t= %d\n", sizeof(char_pointer));
  printf("elements of char_pointer= %d\n", sizeof(char_pointer) / sizeof(char));
  printf("strlen(char_pointer)\t= %d\n", strlen(char_pointer));
  puts("");

  puts("int array:");
  printf("sizeof(int_array)\t= %d\n", sizeof(int_array));
  printf("elements of int_array\t= %d\n", sizeof(int_array) / sizeof(int));
  puts("");

  puts("char array:");
  printf("sizeof(char_array)\t= %d\n", sizeof(char_array));
  printf("elements of char_array\t= %d\n", sizeof(char_array) / sizeof(char));
  printf("strlen(char_array)\t= %d\n", strlen(char_array));
  puts("");

  puts("as you can see:\n\t1. sizeof() has NO knowledge about allocated "
       "space.\n\t2. sizeof() has NO knowledge about sizes of arrays in a "
       "called function");

  // call the function with pointers
  puts("func with pointers");
  func(char_pointer, CHAR_SIZ, int_pointer, INT_SIZ);

  puts("func with arrays");
  func(char_array, CHAR_SIZ, int_array, INT_SIZ);

  // free
  free(int_pointer);
  free(char_pointer);

  puts("READY.");
  exit(EXIT_SUCCESS);
}
