/***
    Sizeof

    char[]
    sizeof()/sizeof(char) here gives the the used space (statically allocated)
    strlen() returns only the space used by characters WITHOUT the '\0'.

    char*
    sizeof() here returns only the size of the pointer - only of the one token
it points to!

    "" terminates autmatically with '\0'

    strlen() never counts '\0'

***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_LENGTH 8

extern size_t strlen(const char *);

int main(int argc, char **argv)
{
  char var = 'L';
  char *pVar = &var;
  char arr1[ARR_LENGTH] = {'a', 'b', 'c',
                           'd'}; // doesn't contain '\0' but may append '\0' !!!
  char arr2[ARR_LENGTH];
  char *pArr1;
  if ((pArr1 = malloc(ARR_LENGTH * sizeof(char *))) == NULL)
    return -1;

  // vanilla output
  printf("Using \t\t\tsizeof siz/siz strlen content\n\n");
  printf("char var\t\t- %d\t%d\t%d\t#%c#\n", sizeof(var),
         sizeof(var) / sizeof(char), strlen(&var), var);
  printf("char* pVar\t\t- %d\t%d\t%d\t#%s#\n", sizeof(pVar),
         sizeof(pVar) / sizeof(char *), strlen(pVar), pVar);
  printf("char arr1[init]\t\t- %d\t%d\t%d\t#%s#\n", sizeof(arr1),
         sizeof(arr1) / sizeof(char), strlen(arr1), arr1);
  printf("char arr2[empty]\t- %d\t%d\t%d\t#%s#\n", sizeof(arr2),
         sizeof(arr2) / sizeof(char), strlen(arr2), arr2);
  printf("char* pArr1 on x\t- %d\t%d\t%d\t#%s#\n", sizeof(pArr1),
         sizeof(pArr1) / sizeof(char *), strlen(pArr1), pArr1);

  // init
  // var - set
  pVar = NULL;
  arr1[4] = 'e';
  arr1[5] = 'f';
  arr1[6] = 'g';
  arr1[7] = 'h'; // leaving the termination '\0' open!
  arr2[0] = 'a'; // no other way to set a char[] after init!
  arr2[1] = 'b';
  arr2[2] = 'c';
  arr2[3] = 'd';  // no termination!
  pArr1 = "abcd"; // the " terminate the string with '\0' which isn't read by
                  // strlen()!!!

  // output after init
  printf("\n\nAfter init:\n\n");
  printf("char var\t\t- %d\t%d\t%d\t#%c#\n", sizeof(var),
         sizeof(var) / sizeof(char), strlen(&var), var);
  printf("char* pVar\t\t Segmentation Fault - NULL!!!\n");
  printf("char arr1[init]\t\t- %d\t%d\t%d\t#%s#\n", sizeof(arr1),
         sizeof(arr1) / sizeof(char), strlen(arr1), arr1);
  printf("char arr2[init]\t\t- %d\t%d\t%d\t#%s#\n", sizeof(arr2),
         sizeof(arr2) / sizeof(char), strlen(arr2), arr2);
  printf("char* pArr1 on x\t- %d\t%d\t%d\t#%s#\n", sizeof(pArr1),
         sizeof(pArr1) / sizeof(char *), strlen(pArr1), pArr1);

  return 0;
}
