// endian.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * checks if the system is a little or a big endian
 * AA  BB  CC  DD    -    little endian
 * EE  FF  11  22
 *
 * EE  CC  DD  22    -    big endian
 * 22  22  FF  EE
//*/

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int WORD;
typedef unsigned char BYTE;


int main(int argc, char **argv)
{
  WORD word = 0x22CCDDEE;
  BYTE *byte = ( BYTE * )&word;
  if (byte[0] == ((word >> 0) & 0xFF))
    printf("little-endian architecture\n");
  if (byte[0] == ((word >> 24) & 0xFF))
    printf("big-endian architecture\n");

  return EXIT_SUCCESS;
};
