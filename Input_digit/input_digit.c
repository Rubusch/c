// input_digit.c
/*
  demonstrates reading in a single digit
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void readdigit(unsigned int*, const char*);


int main(int argc, char** argv)
{
  // read in one digit and transform it into an int: fgets()
  puts("\nSTANDARD WAY TO READ IN ONE CHARACTER");
  unsigned int iChr = 0;
  readdigit(&iChr, "1. enter a one digit number");
  printf("the number of one digit was '%d'\n", iChr);

  puts("READY.");
  exit(EXIT_SUCCESS);
}


// READ
void readdigit(unsigned int* iVal, const char* comment)
{
  char cChr[3];
  do{
    // reset
    memset(cChr, '\0', 3);
    puts(comment);

    // read
    fgets(cChr, 3, stdin);

    // check stdin and clean in case input exceeded the
    if('\n' == cChr[1]){
      // ok - do nothing here

    }else if(('\n' == cChr[0]) || ('\n' == cChr[2])){
      // number too long or input empty - erease
      memset(cChr, '\0', 3);

    }else{
      // '\n' still in stdin - clean up
      puts("input too long - will reset");
      memset(cChr, '\0', 3);
      while('\n' != fgetc(stdin));
    }

    // CAUTION: due to '\n' checking the set of '\0' here!
    cChr[2] = '\0';

    // number?
  }while(!isdigit(cChr[0]));

  // transform
  *iVal = atoi(cChr);
}

