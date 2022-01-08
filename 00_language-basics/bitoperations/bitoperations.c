// bitoperations.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * bitoperations and bitoperations
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "lotharlib/console_input.h"


/*
  pOutSize needs one additional digit for '\0'
 */
void bit_print_int(int var, char *pOut, const unsigned int pOutSize)
{
	unsigned int bitmask = 1;
	unsigned int pos = pOutSize - 1;
	memset(pOut, '*', pOutSize);
	pOut[pos] = '\0';

	for (pos = pos - 1; 0 < bitmask; --pos) {
		// comparte and write
		pOut[pos] = ((var & bitmask) ? '1' : '0');

		// generate bitmask
		bitmask <<= 1;
	}
}

void getVal(unsigned int *val)
{
	unsigned long int tmp = 0;
	do {
		readlongnumber(&tmp, 5, "enter a valid unsigned int value:");
	} while (INT_MAX < tmp);
	*val = (unsigned int)tmp;
}

#define outSize  (1 + ((sizeof(int) * 16)) / 2)
int main(int argc, char **argv)
{
	puts("bit operation tester\n");

	unsigned int intA = 0;
	unsigned int intB = 0;
	unsigned long int iResult;

	puts("Value A");
	getVal(&intA);
	puts("\n");

	puts("Value B");
	getVal(&intB);
	puts("\n");

	fprintf(stderr, "digits: %d\n", outSize - 1);

	char outA[outSize];
	memset(outA, '\0', outSize);

	char outB[outSize];
	memset(outB, '\0', outSize);

	unsigned int outSeparatorSize = 8 + outSize;
	char outSeparator[outSeparatorSize];
	memset(outSeparator, '-', outSeparatorSize);
	outSeparator[outSeparatorSize - 1] = '\0';

	char outResult[outSize];
	memset(outResult, '\0', outSize);

	bit_print_int(intA, outA, outSize);
	bit_print_int(intB, outB, outSize);
	fprintf(stderr, "variables:\nA = %s\t%d\nB = %s\t%d\n", outA, intA,
		outB, intB);
	puts("\n");

	// Exponent: 2 << 2
	bit_print_int((intA << intB), outResult, outSize);
	iResult = intA << intB;
	iResult = ((iResult > 0) && (iResult < INT_MAX)) ? iResult : 0;
	fprintf(stderr, "\t%s = %d\n<<\t%s = %d\n%s\n \t%s = %ld\n", outA, intA,
		outB, intB, outSeparator, outResult, iResult);
	puts("\n");

	// Divide by 2: 2 >> 2
	bit_print_int((intA >> intB), outResult, outSize);
	iResult = intA >> intB;
	iResult = ((iResult > 0) && (iResult < INT_MAX)) ? iResult : 0;
	fprintf(stderr, "\t%s = %d\n>>\t%s = %d\n%s\n\t%s = %ld\n", outA, intA,
		outB, intB, outSeparator, outResult, iResult);
	puts("\n");

	// EX OR: 2^2
	bit_print_int((intA ^ intB), outResult, outSize);
	fprintf(stderr, "\t%s = %d\n^\t%s = %d\n%s\n\t%s = %d\n", outA, intA,
		outB, intB, outSeparator, outResult,
		(unsigned int)(intA ^ intB));
	puts("\n");

	// bitmask: OR
	bit_print_int((intA & intB), outResult, outSize);
	fprintf(stderr, "\t%s = %d\n&\t%s = %d\n%s\n\t%s = %d\n", outA, intA,
		outB, intB, outSeparator, outResult,
		(unsigned int)(intA & intB));
	puts("\n");

	// bitmask: AND
	bit_print_int((intA | intB), outResult, outSize);
	fprintf(stderr, "\t%s = %d\n|\t%s = %d\n%s\n\t%s = %d\n", outA, intA,
		outB, intB, outSeparator, outResult,
		(unsigned int)(intA | intB));
	puts("\n");

	exit(EXIT_SUCCESS);
}
