/*
  floating point issues - real fun starts now!
*/

#include <stdio.h>
#include <float.h>

int main()
{
	fprintf(stderr, "float and double - size and limits\n\n");

	{
		fprintf(stderr, "float: sizeof(float) = %d\n", sizeof(float));
		fprintf(stderr, "float: max float = %f\n", FLT_MAX);

		fprintf(stderr, "double: sizeof(double) = %d\n", sizeof(double));
		fprintf(stderr, "double: max double = %f\n", DBL_MAX);

		fprintf(stderr, "long double: sizeof(long double) = %d\n", sizeof(long double));
		fprintf(stderr, "long double: max double = %Lf\n", LDBL_MAX);
	}

	{
		fprintf(stderr, "comparison: epsilon approach\n");
		fprintf(stderr, "FLT_EPSILON = %1.15f\n", FLT_EPSILON);
		fprintf(stderr, "DBL_EPSILON = %1.31f\n", DBL_EPSILON);
		fprintf(stderr, "LDBL_EPSILON = %1.63Lf\n", LDBL_EPSILON);
	}

	{
		fprintf(stderr, "mantissa:\n");
		fprintf(stderr, "FLT_MANT_DIG = %d\n", FLT_MANT_DIG);
		fprintf(stderr, "DBL_MANT_DIG = %d\n", DBL_MANT_DIG);
	}

	fprintf(stderr, "READY.\n");
	return 0;
}
