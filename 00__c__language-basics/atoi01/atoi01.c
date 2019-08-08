// atoi01.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void output(char*, char*, char*);


int main(char** argv, int argc)
{
  char arr1[] = "foo";
  char arr2[] = "bar";
  char arr3[] = "BAR";

  printf("Arrays: %s, %s and %s\n", arr1, arr2, arr3);

  // strings
  printf("\twithout atoi:\n");
  output(&arr1[0], &arr2[0], &arr3[0]);
  printf("\n");

  // extracting letters
  printf("\tFirst character of %s is %c (%i)\n", arr1, arr1[0], arr1[0]);
  printf("\tFirst character of %s is %c (%i)\n", arr2, arr2[0], arr2[0]);
  printf("\tFirst character of %s is %c (%i)\n", arr3, arr3[0], arr3[0]);
  printf("\n");

  // letters
  char arr4[2] = {arr1[0], '\0'}; //  char arr4[2] = {'f', '\0'};
  char arr5[2] = {arr2[0], '\0'};
  char arr6[2] = {arr3[0], '\0'};

  printf("New arrays: %s, %s and %s\n", arr4, arr5, arr6);
  output(arr4, arr5, arr6);
  printf("\n");

  // presentation of arr4 - "f"
  printf("presentation of arr4 - \"f\"\n");
  printf("\t%c\n", arr4);            // U
  printf("\t%lu\n", arr4);            // 2280666 - address
  printf("\t%lu\n", (unsigned long) arr4);      // 2280666 - same, just the adress
  printf("\t%i\n", *arr4);           // 102     - ASCII enconding
  printf("\t%s\n", arr4);            // f       - character under ASCII
  printf("\t%i\n", atoi(arr4));      // 0       - 'f' is not a representation of a dec number
  printf("\n");

  // presentation of the first character of arr4 - 'f'
  printf("presentation of arr4[0] - \'f\'\n");
  printf("\t%c\n", &arr4[0]);        // U
  printf("\t%lu\n", &arr4[0]);        // 2280666
  printf("\t%lu\n", (unsigned long) &arr4[0]);  // 2280666
  printf("\t%i\n", arr4[0]);         // 102
  printf("\t%s\n", &arr4[0]);        // f
  printf("\t%i\n", atoi(&arr4[0]));  // 0
  printf("\n");

  // isupper, islower
  printf("isupper() and islower()\n");
  printf("\tislower(\'%s\'): %i\n", arr4, islower(arr4[0]));
  printf("\tisupper(\'%s\'): %i\n", arr4, isupper(arr4[0]));
  printf("\n");

  printf("READY.\n");

  return 0;
};


void output(char* arr1, char* arr2, char* arr3){
  if(arr1 > arr2) printf("\t%i. %s (%lu) > %s (%lu)\n", 1, arr1, arr1, arr2, arr2);
  else printf("\t%i. %s (%i) <= %s (%i)\n", 1, arr1, arr1, arr2, arr2);

  if(arr2 > arr3) printf("\t%i. %s (%lu) > %s (%lu)\n", 2, arr2, arr2, arr3, arr3);
  else printf("\t%i. %s (%i) <= %s (%i)\n", 2, arr2, arr2, arr3, arr3);

  if(arr1 > arr3) printf("\t%i. %s (%lu) > %s (%lu)\n", 3, arr1, arr1, arr3, arr3);
  else printf("\t%i. %s (%i) <= %s (%i)\n", arr1, arr1, arr3, arr3);
};
