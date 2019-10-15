// console_input.c
/*
  console input functions - implementation
//*/

#include "consoleinput.h"


int isnumber(const char *szNum, const unsigned int szNum_size)
{
  // TODO: check if a passed character szNum is a valid number -> return 1, or
  // not -> return 0
  return 1;
}


void readstring(char *cTxt, const unsigned int txtSize, const char *comment)
{
  if (NULL == comment) {
    perror("text is NULL");
    return;
  }
  if (NULL == cTxt) {
    perror("iTxt is NULL");
    return;
  }

  do {
    // reset
    // TODO

    // read in
    register unsigned int c;
    register unsigned int idx = 0;

    // in case of exceeding the size of the variable - put a '\0' at the end
    // and read until '\n', but don't store the characters for cleaning the
    // stream
    for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
      if ((txtSize - 1) > idx)
        cTxt[idx] = c;
      else if ((txtSize - 1) == idx) {
        puts("input too long - will be reset");
        memset(cTxt, '\0', txtSize);
        // or cut here:
        // cTxt[idx] = '\0';
      }
    }

  } while (0 == strlen(cTxt));
}


void readdigit(unsigned int *iVal, const char *comment)
{
  char cChr[3];
  do {
    // reset
    memset(cChr, '\0', 3);
    printf("%s\n> ", comment);

    // read
    fgets(cChr, 3, stdin);

    // check stdin and clean in case input exceeded the
    if ('\n' == cChr[1]) {
      // TODO

    } else if (('\n' == cChr[0]) || ('\n' == cChr[2])) {
      // TODO

    } else {
      // TODO
    }

    // CAUTION: due to '\n' checking the set of '\0' here!
    cChr[2] = '\0';

    // number?
  } while (!isdigit(cChr[0]));

  // transform
  *iVal = atoi(cChr);
}


void readnumber(unsigned int *iVal, const unsigned int digits,
                const char *comment)
{
  if (NULL == comment) {
    perror("text is NULL");
    return;
  }
  if (NULL == iVal) {
    perror("iVal is NULL");
    return;
  }

  unsigned int size = digits + 1;
  char cTxt[size];
  do {
    // reset
    // TODO

    // read in
    register unsigned int c;
    register unsigned int idx = 0;

    // in case of exceeding the size of the variable - put a '\0' at the end
    // and read until '\n', but don't store the characters for cleaning the
    // stream
    for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
      // TODO
    }
  } while (!isnumber(cTxt, (1 + strlen(cTxt))));
  *iVal = atoi(cTxt);
}
