/*
  math

  DON'T USE FUNCTION CALLS IN MACRO CALLS
  THIS RESULTS IN DOUBLED FUNCTION CALLS!!!

  some code to simplify the life in C

  This code is GPL!
  Author: Lothar Rubusch, 2008
*/

#ifndef DEVKIT_LIB_MATH
#define DEVKIT_LIB_MATH

/*
  min / max - improve using __typeof__
*/
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

// return the sum
#define SUM(x, y) x + y


#endif /* DEVKIT_LIB_MATH */
