// input.c
/*
  demonstrates a switched input

  @author: Lothar Rubusch
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int isnumber(const char*, const unsigned int);
void readdigit(unsigned int*, const char*);


// TOOL
int isnumber(const char* str, const unsigned int size)
{
  char arr[size];
  if (0 == size) return 0;
  memset(arr, '\0', size);
  strncpy(arr, str, size);
  arr[size-1] = '\0';

  const unsigned int len = strlen(arr);
  if(0 == len) return 0;

  int idx;
  for(idx=0; idx < len; ++idx){
    if( ('\n' == arr[idx]) || ('\0' == arr[idx])) break;
    if( !isdigit( arr[idx] )) return 0;
  }
  return 1;
}


// READ
void readdigit(unsigned int* iChr, const char* comment)
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
      if (-1 == fgetc(stdin)) {
        perror("input was control character");
        exit(EXIT_FAILURE); // EOT, ENQ, ...
      }
      puts("input too long - will reset");
      memset(cChr, '\0', 3);
      while('\n' != fgetc(stdin));
    }

    // CAUTION: due to '\n' checking the set of '\0' here!
    cChr[2] = '\0';

    // number?
  }while(!isdigit(cChr[0]));

  // transform
  *iChr = atoi(cChr);
}


int main(int argc, char** argv)
{
  puts("READ A DIGIT FOR A SWITCH CASE SITUATION");

  unsigned int run = 1;
  unsigned int val = 0;
  do{
    // init
    val = 0;

    // read
    readdigit(&val, "enter 1 or 2 ('0' to quit):");

    switch (val){
    case 0:
      puts("'0' - exiting");
      run = 0;
      break;

    case 1:
      puts("input was '1'");
      break;

    case 2:
      puts("input was '2'");
      break;

    default:
      puts("false input - try again");
      break;
    }

  }while(run);

  puts("READY.");
  exit(EXIT_SUCCESS);
}
