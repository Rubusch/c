// input_number_alternative.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-May-01

  demonstrates the reading from the console, it reads a text and turns
  it into a valid integer value or continues asking

  an alternative method reading in from stdin by fgets, the big problem
  here is the '\n'
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// TXT_SIZE - 2 digits, for "\n\0"
#define TXT_SIZE 10

int isnumber(const char*, const unsigned int);
void readnumber_fgets(unsigned int*, const unsigned int, const char*);


// TOOL
// returns
// 0 == is number
// 1 == is not a number
int isnumber(const char* str, const unsigned int size)
{
  char arr[size];

  if (0 == size) return 0; // number was too long, thus size was zero
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


// READ number, several digits
void readnumber_fgets(unsigned int* iVal, const unsigned int digits, const char* comment)
{
  if(NULL == comment){
    perror("text is NULL");
    return;
  }
  if(NULL == iVal){
    perror("iVal is NULL");
    return;
  }

  unsigned int size = digits+2;
  char cTxt[size];
  do{
    // reset
    memset(cTxt, '\0', size);
    puts(comment);

    // read in
    fgets(cTxt, size, stdin);

    // check if the read numbers contain '\n'
    unsigned int idx=0;
    for(idx=0; size > idx; ++idx)
      if('\n' == cTxt[idx]) break;

    // input didn't contain '\n' - clean stdin and reset input
    if((size-1) <= idx){
      puts("input too long - will be reset");
      memset(cTxt, '\0', size);
      while('\n' != fgetc(stdin));
    }

    // CAUTION: due to '\n' check finalization here
    cTxt[size-1] = '\0';
  }while(!isnumber(cTxt, strlen(cTxt)));
  *iVal = atoi(cTxt);
}


int main(int argc, char** argv)
{
  // read in string and transform it into a number: fgets()
  puts("\nALTERNATIVE POSSIBILITY TO READ IN A STRING");
  unsigned int iTxt2 = 0;
  readnumber_fgets(&iTxt2, TXT_SIZE, "enter a number up to 10 digits");
  printf("the number was '%d'\n", iTxt2);

  puts("READY.");
  exit(EXIT_SUCCESS);
}
