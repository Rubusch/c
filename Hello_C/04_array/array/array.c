// array.c
/*
  char arrays (I recommend the following - just my own termination, but is similar to e.g. strLEN())

  size      the number of elements, inclusive the '\0'

  length    the number of only the characters (thus exclusive the '\0')

  index     the index of each element, the index starts with 0
            the index of all elements starts at 0 and goes until idx < size
            the index of all characters starts at 0 and goes until idx < size-1 
	    or idx < length - which is the last index

output:
$ ./array.exe
arrays
arr1:
content of the array:
        address,        index           : content
        ------------------------------------------
        0x22ccd0,       element 0       : "t"
        0x22ccd1,       element 1       : "e"
        0x22ccd2,       element 2       : "n"
        0x22ccd3,       element 3       : "t"
        0x22ccd4,       element 4       : "o"
        0x22ccd5,       element 5       : "k"
        0x22ccd6,       element 6       : "e"
        0x22ccd7,       element 7       : "n"
        0x22ccd8,       element 8       : "s"
arr1 length: 9, size: 10

arr2:
content of the array:
        address,        index           : content
        ------------------------------------------
        0x22ccc0,       element 0       : "a"
        0x22ccc1,       element 1       : "n"
        0x22ccc2,       element 2       : "o"
        0x22ccc3,       element 3       : "t"
        0x22ccc4,       element 4       : "h"
        0x22ccc5,       element 5       : "e"
        0x22ccc6,       element 6       : "r"
        0x22ccc7,       element 7       : "w"
        0x22ccc8,       element 8       : "o"
        0x22ccc9,       element 9       : "r"
        0x22ccca,       element 10      : "d"
arr2 length: 11, size: 12

READY.

//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZ 10


void print_content(char* arr, unsigned int arr_siz)
{
  if(NULL == arr) return;

  puts("content of the array:");
  puts("\taddress,\tindex\t\t: content");
  puts("\t------------------------------------------");
  // do a recommendation to "register" to gain performance for "running indexes" - 
  // only possible if the variable (here idx) won't be "referenced" (=used with a '&')
  register unsigned int idx=0;
  for(idx=0; idx < arr_siz; ++idx){
    printf("\t0x%lx,\telement %d\t: \"%c\"\n", (unsigned long) &arr[idx], idx, arr[idx]);
  }
}


int main()
{ 
  puts("arrays");

  // init
  char arr1[ARRAY_SIZ];
  strncpy(arr1, "tentokens", ARRAY_SIZ);
  char arr2[] = "anotherword";
  
  // print length:
  puts("arr1:");
  print_content(arr1, ARRAY_SIZ-1);
  printf("arr1 length: %d, size: %d\n", strlen(arr1), (sizeof(arr1) / sizeof(char)));
  puts("");

  puts("arr2:");
  print_content(arr2, (sizeof(arr2) / sizeof(char)) - 1);
  printf("arr2 length: %d, size: %d\n", strlen(arr2), (sizeof(arr2) / sizeof(char)));
  puts("");

  puts("READY.");
  exit(EXIT_SUCCESS);
}
