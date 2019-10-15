/*
  mathtools

  DON'T USE FUNCTION CALLS IN MACRO CALLS
  THIS RESULTS IN DOUBLED FUNCTION CALLS!!!

  some code to simplify the life in C

  This code is GPL!
  Author: Lothar Rubusch, 2008
//*/

#ifndef MATHTOOLS
#define MATHTOOLS

// return the greatest element
#define MAX(x, y) ((x > y) ? (x) : (y))

// return the smallest element
#define MIN(x, y) ((x < y) ? (x) : (y))

// return the sum
#define SUM(x, y) x + y


#endif
