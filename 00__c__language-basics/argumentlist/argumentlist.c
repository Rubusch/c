// argumentlist.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * demonstrates the use of an argument list with variable size
//*/

#include <stdarg.h>
#include <stdio.h>

double sum(int, ...);


int main(int argc, char **argv)
{
  double d = sum(5, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6);
  printf("d = %f\n", d);
}


double sum(int numberOfArguments, ...)
{
  double result = 0.0;

  // define va_list
  va_list argumentpointer;

  // initialize argumentpointer
  va_start(argumentpointer, numberOfArguments);

  // sum up values
  int cnt = 0;
  for (cnt = 0; cnt < numberOfArguments; ++cnt) {
    result += va_arg(argumentpointer, double);
  }

  // over and out
  va_end(argumentpointer);

  return result;
}
