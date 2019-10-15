// input_digit.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-May-01

  demonstrates reading in a single digit
//*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readdigit(unsigned int *, const char *);


// READ a dingle digit
// check for empty i.e. '\n'
// check for characters
// check for length
void readdigit(unsigned int *iVal, const char *comment)
{
  char cChr[3];
  do {
    // reset
    memset(cChr, '\0', 3);
    puts(comment);

    // read
    fgets(cChr, 3, stdin);

    // check stdin and clean in case input exceeded the
    if ('\n' == cChr[1]) {
      // ok - do nothing here

    } else if (('\n' == cChr[0]) || ('\n' == cChr[2])) {
      // number too long or input empty - erease
      memset(cChr, '\0', 3);

    } else {
      // '\n' still in stdin - clean up
      puts("input too long - will reset");
      memset(cChr, '\0', 3);
      while ('\n' != fgetc(stdin))
        ;
    }

    // CAUTION: due to '\n' checking the set of '\0' here!
    cChr[2] = '\0';

    // number?
  } while (!isdigit(cChr[0]));

  // transform
  *iVal = atoi(cChr);
}


int main(int argc, char **argv)
{
  // read in one digit and transform it into an int: fgets()
  puts("\nSTANDARD WAY TO READ IN ONE CHARACTER");
  unsigned int iChr = 0;
  readdigit(&iChr, "1. enter a one digit number");
  printf("the number of one digit was '%d'\n", iChr);

  puts("READY.");
  exit(EXIT_SUCCESS);
}
