// Memfoo

/*
        Demonstrates usage the memxyz() functions.

        void* memchr(const void* buffer, int ch, size_t count);
        int memcmp(const void* buf1, const void* buf2, size_t count);
        void* memcpy(void* to, const void* from, size_t count);
        void* memmove(void* to, const void* from, size_t count);
        void* memset(void* buf, int ch, size_t count);
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT1 "No tears for the creatures of the night!"
#define TEXT2 "creatures"
#define TEXT3 "foo"

extern void *memchr(const void *, int, size_t);
extern int memcmp(const void *buf1, const void *buf2, size_t count);
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
extern size_t strlen(const char *);
extern char *strcpy(char *, const char *);


int callMemchr(char *pText, char *pPattern, const unsigned int TEXT_SIZE,
               char **pResult)
{
  printf("\t*** callMemchr() ***\n");

  // init
  if (pText == NULL)
    return -1;
  if (pPattern == NULL)
    return -1;

  // set *pResult on the first occurance of pPattern in pText or return -1
  *pResult = memchr(pText, *pPattern, TEXT_SIZE);
  if (*pResult == NULL)
    return -1;

  return 0;
}


int callMemcmp(const char *pText1, const char *pText2,
               const unsigned int TEXT_SIZE, int *iResult)
{
  printf("\t*** callMemcmp() ***\n");

  // init
  if (iResult == NULL)
    return -1;

  *iResult = memcmp(pText1, pText2, TEXT_SIZE);

  return 0;
}


// only the content changes, therefore char* are sufficient, since the result
// changes address, a char** is necessary!
int callMemcpy(char *pText1, const char *pText2, int TEXT_SIZE, char **pResult)
{
  printf("\t*** callMemcpy() ***\n");

  if (pText1 == NULL)
    return -1;
  if (pText2 == NULL)
    return -1;

  *pResult = memcpy(pText1, pText2, TEXT_SIZE);
  if (*pResult == NULL)
    return -1;

  return 0;
}

// same as memcpy() but works for overlapping arrays (though the second array
// will be modified!)
int callMemmove(char *pText1, char *pText2, int TEXT_SIZE, char **pResult)
{
  printf("\t*** callMemmove() ***\n");

  if (pText1 == NULL)
    return -1;
  if (pText2 == NULL)
    return -2;

  *pResult = memmove(pText1, pText2, TEXT_SIZE);
  if (*pResult == NULL)
    return -1;

  return 0;
}


int callMemset(char *pText, int ch, const unsigned int TEXT_SIZE,
               char **pResult)
{
  printf("\t*** callMemset() ***\n");

  if (pText == NULL)
    return -1;

  *pResult = memset(pText, ch, TEXT_SIZE);
  if (*pResult == NULL)
    return -1;

  return 0;
}


int freeAll(char **pText1, char **pText2, char **pText3)
{
  if (*pText1 != NULL)
    free(*pText1);
  if (*pText2 != NULL)
    free(*pText2);
  if (*pText3 != NULL)
    free(*pText3);

  return 0;
}


int main(int argc, char **argv)
{
  // init
  printf("Memfoo() - init\n\n");
  char *text1 = NULL;
  if ((text1 = malloc(sizeof(char) * (strlen(TEXT1) + 1))) == NULL)
    return -1;
  strcpy(text1, TEXT1);
  const unsigned int TEXT1_SIZE = (strlen(text1) + 1);

  char *text2 = NULL;
  if ((text2 = malloc(sizeof(char) * (strlen(TEXT2) + 1))) == NULL)
    return -1;
  strcpy(text2, TEXT2);
  const unsigned int TEXT2_SIZE = (strlen(text2) + 1);

  char *text3 = NULL;
  if ((text3 = malloc(sizeof(char) * (strlen(TEXT3) + 1))) == NULL)
    return -1;
  strcpy(text3, TEXT3);


  char character = 'c';
  char *pResult = NULL;
  int iResult;


  /*
    memchr() - searches the first occurance of a character "ch" in "count"
  characters in "buffer" returns a pointer on this first occurance or NULL ptr
  //*/
  printf("void* memchr(const void* buffer, int ch, size_t count)\n");
  pResult = NULL;
  printf("\tString:\t\"%s\" \n\tcontains \"%c\"?\n", text1, character);
  if (0 == callMemchr(text1, &character, TEXT1_SIZE, &pResult))
    printf("\tYes: \"%s\"\n", pResult);
  else
    printf("\tNo!");
  printf("\n");


  /*
    memcmp() - compares the first "count" characters in "buf1" and "buf2"
    returns:
    <0 - buf1 smaller than buf2
    0  - buf1 equals buf2
    >0 - buf1 greater than buf2
  //*/
  printf("int memcmp(const void* buf1, const void* buf2, size_t count)\n");
  pResult = NULL;
  printf("\tCompare text1 with text1: %i\n",
         callMemcmp(text1, text1, TEXT1_SIZE, &iResult));
  pResult = NULL;
  printf("\tCompare text1 with text2: %i\n",
         callMemcmp(text1, text2, TEXT1_SIZE, &iResult));
  pResult = NULL;
  printf("\tCompare text3 with text2: %i\n",
         callMemcmp(text3, text2, TEXT1_SIZE, &iResult));
  printf("\n");


  /*
    memmove() - copies "count" characters from "from" to the array "to"
    in case of overlapping arrays, memmove() the array "to" will contain the
  correct output, but "from" will be modified returns a pointer to "to" or NULL
  //*/
  printf("void* memmove(void* to, const void* from, size_t count)\n");
  char *pTemp = pResult;
  pResult = NULL;
  printf("\tbefore:\n\ttext1 = \"%s\"\n\ttext2 = \"%s\"\n\tResult = NULL\n",
         text1, pTemp);
  callMemmove(text1, pTemp, TEXT1_SIZE, &pResult);
  printf("\tafter:\n\ttext1 = \"%s\"\n\ttext2 = \"%s\"\n\tResult = \"%s\"\n",
         text1, pTemp, pResult);
  printf("\n");


  /*
    memcpy() - copies "count" characters from "from" to the array "to"
    in case of overlapping arrays, memcpy() is undefined
    returns a pointer to "to" or NULL
  //*/
  printf("void* memcpy(void* to, const void* from, size_t count)\n");
  pResult = NULL;
  printf("\tbefore:\n\ttext1 = \"%s\"\n\ttext2 = \"%s\"\n\tResult = NULL\n",
         text1, text2);
  callMemcpy(text1, text2, TEXT1_SIZE, &pResult);
  printf("\tafter:\n\ttext1 = \"%s\"\n\ttext2 = \"%s\"\n\tResult = \"%s\"\n",
         text1, text2, pResult);
  printf("\n");


  /*
    memset() - copies the low-order-byte of "ch" into the first "count"
  characters of the array "buf" the low-order-byte is the eight bits in a 16 bit
  word, farthest to the right is used to initialize an array to '0' or '\0'
    returns a pointer to "buf" or NULL
  //*/
  printf("void* memset(void* buf, int ch, size_t count)\n");
  pResult = NULL;
  printf("\tBefore: \"%s\"\n", text2);
  callMemset(text2, character, TEXT2_SIZE, &pResult);
  printf("\tAfter: \"%s\"\n", text2);
  printf("\n");


  printf("READY.\n");
  freeAll(&text1, &text2, &text3);
  return EXIT_SUCCESS;
}
