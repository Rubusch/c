//*/ preprocessor.c

// What are preprocessor directives good for?

// 1. Include guard
#ifndef JUST_A_RANDOM_TEXT
#define JUST_A_RANDOM_TEXT
// -> needs #endif at the end
// with an include guard you make sure that the file will only be read
// once during compiling - it is necessary to prevent errors.
// the random text will be defined and the next time a #include "mycode.h"
// is fired, the already defined text prevents the compilation of that code.

#include <stdio.h>
#include <stdlib.h>

// 2. for debugging:
#define DEBUG
// if the line above is commented out, the printf below (in #ifdef... and
// #endif) won't be compiled -> no output, try it!

// 3. substitude cryptic code e.g. (the itention is to facilitate the work,
// since it is also possible to screw it up!):
#define MAX(x, y) ((x > y) ? (1) : (0))

// 4. define a constant value, that can e.g. also be used for fixed sizes of
// static arrays
#define ARRAYSIZE 2
#define TRUE "certo!"
#define FALSE "errado!"

int main(char **argv, int argc)
{
  int arr[ARRAYSIZE];
  arr[0] = 10;
  arr[1] = 5;

#ifdef DEBUG
  // belongs to 2.
  printf("Debugging output: a = %i, b = %i\n", arr[0], arr[1]);
#endif

  printf("Is %i greater than %i? %s\n", arr[0], arr[1],
         ((MAX(arr[0], arr[1])) ? (TRUE) : (FALSE)));

  return EXIT_SUCCESS;
};
#endif


/*
  How things work:
  code -> PREPROCESSOR -> COMPILER -> assembler -> linker -> runnable/binary

  the preprocessor basically tells the compiler which parts of the code to
translate.
  - all preprocessor directives start with '#'
  - using #pragma you can set compiler specific options for the compilation
  - preprocessor directives tell the compiler where to look for code to include
(e.g. #include <xyz.h>)
  - preprocessor directives tell the compiler to replace defined tags by other
code (macros e.g. #define)
  - preprocessor directives tell the compiler to translate only certain partes
of the code file xyz.c (e.g. #ifdef, #elif, #endif)

  Macros have the syntax:
  #define <new name> <text to replace>
  or with parameters
  #define BLAHBLAH(x,y) (something with x and y)

  The preprocessor REPLACES the text with the proggy above.

  Attention:
  -> A #define is NO variable!! It can't be changed afterwards.

  -> not to place a ';' after the macro definition it will be treated
  as it belongs to the string to replace, e.g. a:
     #define FOO 123;

  will lead to a:
     func(FOO);

  which is equal to:
     func(123;); // bug!

   -> Macros afaik use less space since they replace the code directly,
   therefore it is possible to define small functions rather using
   macros, like "inlines" which increase performance.

   -> use macros where they make it easier with understandable names...
   blablabla...


It is even possible to redefine a new language (decoment):
//*
//*
#include <stdio.h>
#include <stdlib.h>

#define START int main(int argc, char** argv){
#define PRINT printf("Hello World!\n");
#define END   return EXIT_SUCCESS; }

START
PRINT
END
//*/
