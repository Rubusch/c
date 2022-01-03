/*
  floating point issues - real fun starts now!

  references:
  https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/ by Bruce Dawson, Google
*/

#include <stdio.h>
#include <float.h>
#include <math.h> /* fabs() */

int main()
{
	fprintf(stderr, "float and double - comparing small values\n\n");

	{
		fprintf(stderr, "comparison: epsilon approach\n");
		fprintf(stderr, "FLT_EPSILON = %1.15f\n", FLT_EPSILON);
		fprintf(stderr, "DBL_EPSILON = %1.31f\n", DBL_EPSILON);
		fprintf(stderr, "LDBL_EPSILON = %1.63Lf\n", LDBL_EPSILON);
	}

	{
		float flo;
		flo = 0.1f;

		if (FLT_EPSILON >= fabs(0.1 - flo)) {
			fprintf(stderr, "EQUAL\tflo = %1.15f equals to %1.15f on EPSILON == %f1.15\n", flo, 0.1, FLT_EPSILON);
		} else {
			fprintf(stderr, "!!!\tflo = %1.15f is NOT equal to %1.15f on EPSILON == %f1.15\n", flo, 0.1, FLT_EPSILON);
		}
	}

	{
		double dbl;
		dbl = 0.1l;

		if (DBL_EPSILON >= fabs(0.1 - dbl)) {
			fprintf(stderr, "EQUAL\tdbl = %1.31f equals to %1.31f on EPSILON == %1.31f\n", dbl, 0.1, DBL_EPSILON);
		} else {
			fprintf(stderr, "!!!\tdbl = %1.31f is NOT equal to %1.31f on EPSILON == %1.31f\n", dbl, 0.1, DBL_EPSILON);
		}
	}

	{
		long double ldbl;
		ldbl = 0.1L;

		if (LDBL_EPSILON >= fabs(0.1 - ldbl)) {
			fprintf(stderr, "EQUAL\tldbl = %1.63Lf equals to %1.63f on EPSILON == %1.63Lf\n", ldbl, 0.1, LDBL_EPSILON);
		} else {
			fprintf(stderr, "!!!\tldbl = %1.63Lf is NOT equal to %1.63f on EPSILON == %1.63Lf\n", ldbl, 0.1, LDBL_EPSILON);
		}
	}

	{
		float flo;
		flo = 16777216.1f;

		/*
		  NB: For numbers larger than 2.0 the gap between floats
		  grows larger and if you compare floats using
		  FLT_EPSILON then you are just doing a more-expensive
		  and less-obvious equality check. That is, if two
		  floats greater than 2.0 are not the same then their
		  difference is guaranteed to be greater than
		  FLT_EPSILON. For numbers above 16777216 the
		  appropriate epsilon to use for floats is actually
		  greater than one, and a comparison using FLT_EPSILON
		  just makes you look foolish.
		 */

		if (FLT_EPSILON >= fabs(16777216.1 - flo)) {
			fprintf(stderr, "EQUAL\tflo = %1.15f equals to %1.15f on EPSILON == %f1.15\n", flo, 16777216.1, FLT_EPSILON);
		} else {
			fprintf(stderr, "FAILS [expected] - flo = %1.15f is NOT equal to %1.15f on EPSILON == %1.15f\n", flo, 16777216.1, FLT_EPSILON);
			/*
			  NB: the mantissa can't hold values above the
			  16777216, thus the absolute comparison to
			  FLT_EPSILON results in a larger delta
			*/
		}
	}

	fprintf(stderr, "READY.\n");
	return 0;
}
