/*
  floating point issues - real fun starts now!

  Even a ridiculously small number, like 1e-30 has an ‘large’ integer
  representation of 228,737,632. So, while 1e-30 is ‘close’ to zero
  and ‘close’ to negative 1e-30 it is a gargantuan distance from those
  numbers in ULPs.

  After the special cases are dealt with we simply subtract the
  integer representations, get the absolute value, and now we know how
  different the numbers are. The ‘ulpsDiff’ value gives us the number
  of floats between the two numbers (plus one) which is a wonderfully
  intuitive way of dealing with floating-point error.

  Comparing numbers with ULPs is really just a way of doing relative
  comparisons. It has different characteristics at the extremes, but
  in the range of normal numbers it is quite well behaved. The concept
  is sufficiently ‘normal’ that boost has a function for calculating
  the difference in ULPs between two numbers.

  A one ULP difference is the smallest possible difference between two
  numbers. One ULP between two floats is far larger than one ULP
  between two doubles, but the nomenclature remains terse and
  convenient. I like it.

  NB: ULP based comparisons also have different performance
  characteristics. ULP based comparisons are more likely to be
  efficient on architectures such as SSE which encourage the
  reinterpreting of floats as integers. However ULPs based comparisons
  can cause horrible stalls on other architectures, due to the cost of
  moving float values to integer registers. These stalls generally
  happen when doing float calculations and then immediately doing ULP
  based comparisons on the results, so keep that in mind when
  benchmarking.

  NB: 

  references:
  https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/ by Bruce Dawson, Google
  https://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format/
  https://en.wikipedia.org/wiki/Double-precision_floating-point_format  (double precision notes on wikipedia)
  https://en.wikipedia.org/wiki/C_data_types
*/

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h> /* fabs() */
#include <stdint.h>
#include <stdbool.h>

#include "double_comparison.h"


bool negative(double_wrap_p fp)
{
	return fp->int_repr < 0;
}

int64_t raw_mantissa(double_wrap_p fp)
{
	return fp->int_repr & ((1ULL << (DBL_MANT_DIG-1)) - 1);
}

int64_t raw_exponent(double_wrap_p fp)
{
	return (fp->int_repr >> (DBL_MANT_DIG-1)) & 0xff;
}

/*
  one ULPs difference good (adjacent doubles),
  one million ULPs difference bad (kinda different)

  Normally a difference of one ULP means that the two numbers being
  compared have similar magnitudes - the larger one is usually no
  larger than 1.000000119 times larger than the smaller. But not
  always. Some notable exceptions are:

  - FLT_MAX to infinity - one ULP, infinite ratio
  - zero to the smallest denormal - one ULP, infinite ratio
  - smallest denormal to the next smallest denormal - one ULP,
    two-to-one ratio
  - NaNs - two NaNs could have very similar or even identical
    representations, but they are not supposed to compare as equal
  - Positive and negative zero - two billion ULPs difference, but they
    should compare as equal
  - As discussed above, one ULP above a power of two is twice as big a
    delta as one ULP below that same power of two
 */
bool relatively_equal(double a, double b, int max_ulp_diff)
{
	double_wrap_t aval;
	aval.dbl = a;
	double_wrap_t bval;
	bval.dbl = b;

	if (negative(&aval) != negative(&bval)) {
		if (a == b) return true;
		else return false;
	}

	// find the differences in ulp
	int ulp_diff = abs(bval.int_repr - aval.int_repr);
	if (ulp_diff <= max_ulp_diff)
		return true;
	return false;
}

int main()
{
	fprintf(stderr, "float and double - comparing small values\n\n");

	{
		double dbl;
		dbl = 0.12345f;

		int max_ulp_diff = 500000000; /* for small numbers still 0, gives OK */
		if (relatively_equal(dbl, 0.12345, max_ulp_diff)) {
			fprintf(stderr, "EQUAL\tdbl = %1.15f equals to %1.15f (on ULP with %d)\n", dbl, 0.12345, max_ulp_diff);
		} else {
			fprintf(stderr, "!!!\tdbl = %1.15f is NOT equal to %1.15f (on ULP with %d)\n", dbl, 0.12345, max_ulp_diff);
		}
	}

	{
		double dbl;
		dbl = 228737632.0f;
		double expected;
		expected = 228737635.0;

		int max_ulp_diff = 500000000; /* weaker precision, still ok */

		if (relatively_equal(dbl, expected, max_ulp_diff)) {
			fprintf(stderr, "EQUAL\tdbl = %1.15f equals to %1.15f (on ULP with %d)\n", dbl, expected, max_ulp_diff);
		} else {
			fprintf(stderr, "FAILED!!!\tdbl = %1.15f is NOT equal to %1.15f (on ULP with %d)\n", dbl, expected, max_ulp_diff);
		}
	}

	{
		double dbl;
		dbl = 228737632.0f;
		double expected;
		expected = 228737635.0;

		int max_ulp_diff = 0; /* higher precision: 0, now fails */

		if (relatively_equal(dbl, expected, max_ulp_diff)) {
			fprintf(stderr, "EQUAL\tdbl = %1.15f equals to %1.15f (on ULP with %d)\n", dbl, expected, max_ulp_diff);
		} else {
			fprintf(stderr, "FAILED!!! [expected] dbl = %1.15f is NOT equal to %1.15f (on ULP with %d)\n", dbl, expected, max_ulp_diff);
		}
	}

	fprintf(stderr, "READY.\n");
	return 0;
}
