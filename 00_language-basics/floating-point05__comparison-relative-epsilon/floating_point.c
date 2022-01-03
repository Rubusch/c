/*
  floating point issues - real fun starts now!

  references:
  https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/ by Bruce Dawson, Google
  https://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format/
*/

#include <stdio.h>
#include <float.h>
#include <math.h> /* fabs() */
#include <stdbool.h>

bool relatively_equal(double a, double b)
{
	double max_rel_diff = DBL_EPSILON;
	double diff = fabs(a - b);

	a = fabs(a);
	b = fabs(b);
	double largest = (a > b) ? a : b;

	if (diff <= largest * max_rel_diff)
		return true;

	return false;
}

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
		double dbl;
		dbl = 0.1l;

		/*
		  As a relative approach, this is better than just
		  absolutely compare the difference against
		  EPSILON. Better actually is to go over the "integer
		  representation" (i.e. integer of number and
		  mantissa), and then check the Unit in the Last Place
		  (ULP)
		 */
		if (relatively_equal(dbl, 0.1)) {
			fprintf(stderr, "EQUAL:\tdbl = %1.31f equals to %1.31f on EPSILON == %1.31f\n", dbl, 0.1, DBL_EPSILON);
		} else {
			fprintf(stderr, "!!!\tdbl = %1.31f is NOT equal to %1.31f on EPSILON == %1.31f\n", dbl, 0.1, DBL_EPSILON);
		}
	}


	fprintf(stderr, "READY.\n");
	return 0;
}
