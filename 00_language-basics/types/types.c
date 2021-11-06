#define _GNU_SOURCE /* *_WIDTH */
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

#include <values.h> /* FLOATBITS, DOUBLEBITS */

int main(int argc, char *argv[])
{
	fprintf(stdout, "C types storage sizes:\n\n");

	// alternative CHAR_WIDTH, SCHAR_WIDTH, SCHAR_MIN, SCHAR_MAX
	fprintf(stdout, "%s [%d bit]\t\t: %d\t\t\t... %d\n", "char", CHAR_BIT,
		CHAR_MIN, CHAR_MAX);
#ifdef __GNUC__
	fprintf(stdout, "%s [%d bit]\t\t: %d\t\t\t... %d\n", "uchar",
		UCHAR_WIDTH, 0, UCHAR_MAX);
	fprintf(stdout, "\n");

	fprintf(stdout, "%s [%d bit]\t\t: %d\t\t... %d\n", "short", SHRT_WIDTH,
		SHRT_MIN, SHRT_MAX);
	fprintf(stdout, "%s [%d bit]\t\t: %d\t\t\t... %d\n", "ushort",
		USHRT_WIDTH, 0, USHRT_MAX);
	fprintf(stdout, "\n");

	fprintf(stdout, "%s [%d bit]\t\t: %d\t\t... %d\n", "int", INT_WIDTH,
		INT_MIN, INT_MAX);
	fprintf(stdout, "%s [%d bit]\t\t: %d\t\t\t... %u\n", "uint", UINT_WIDTH,
		0, (unsigned int)UINT_MAX);
	fprintf(stdout, "\n");

	fprintf(stdout, "%s [%d bit]\t\t: %ld\t\t... %ld\n", "long", LONG_WIDTH,
		LONG_MIN, LONG_MAX);
	fprintf(stdout, "%s [%d bit]\t\t: %ld\t\t\t... %lu\n", "ulong",
		ULONG_WIDTH, (long)0, ULONG_MAX);
	fprintf(stdout, "\n");

	fprintf(stdout, "%s [%d bit]\t: %lld\t... %lld\n", "longlong",
		LLONG_WIDTH, LLONG_MIN, LLONG_MAX);
	fprintf(stdout, "%s [%d bit]\t: %lld\t\t\t... %llu\n", "ulonglong",
		ULLONG_WIDTH, (long long)0, ULLONG_MAX);
	fprintf(stdout, "\n");

	// floating point types
	fprintf(stdout, "%s [%d bit]\t\t: %g\t\t... %g\n", "float",
		(int)FLOATBITS, FLT_MIN, FLT_MAX);
	fprintf(stdout, "\n");

	fprintf(stdout, "%s [%d bit]\t\t: %g\t\t... %g\n", "double",
		(int)DOUBLEBITS, DBL_MIN, DBL_MAX);
	fprintf(stdout, "\n");

	// extras
	fprintf(stdout, "float precision value: %d digit\n", FLT_DIG);
	fprintf(stdout, "\n");

	//	fprintf(stdout, "char* size i.e. size of an address: %d bit\n", (sizeof (char*) * CHAR_BIT));
	fprintf(stdout, "char* size i.e. size of an address: %d bit\n",
		CHAR_BIT);
	fprintf(stdout, "\n");
#endif /* __GNUC__ */
	fprintf(stdout, "READY.\n");
	return 0;
}
