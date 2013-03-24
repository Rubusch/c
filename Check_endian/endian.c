// endian.c

/*
  checks if the system is a little or a big endian
  22  CC  DD  EE    -    little endian
  EE  CC  DD  22    -    big endian
//*/

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int WORD;
typedef unsigned char BYTE;


int main(int argc, char** argv)
{
  WORD word = 0x22CCDDEE;
  BYTE* byte = (BYTE*) &word;
  if(byte[0] == ((word >> 0) & 0xFF))
    printf("little-endian architecture\n");
  if(byte[0] == ((word >> 24) & 0xFF))
    printf("big-endian architecture\n");

  return EXIT_SUCCESS;
};
