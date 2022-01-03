/*
  floating point issues - real fun starts now!

  In C/C++ the numbers 0.1 and double(0.1) are the same thing, but
  when I say “0.1” in text I mean the exact base-10 number, whereas
  float(0.1) and double(0.1) are rounded versions of 0.1.

  references:
  https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/ by Bruce Dawson, Google
*/

#include <stdio.h>
#include <float.h>

int main()
{
	fprintf(stderr, "float and double - calculating with float and double expands the error\n");

	{
		double db;
		db = 0.1; /* initialized with no suffix */

		double db_sum;
		db_sum = db + db + db + db + db + db + db + db + db + db;

		double db_sum_assigned;
		db_sum_assigned = 0;
		for (int idx=0; idx<10; idx++)
			db_sum_assigned += db;

		double db_product;
		db_product = 10 * db;

		fprintf(stderr, "expected:\t\t%1.31f\n", 1.0);
		fprintf(stderr, "\timmediate/mul:\t%1.31f (no suffix)\n", (db * 10));
		fprintf(stderr, "\tvariable/sum:\t%1.31f\n", db_sum);
		fprintf(stderr, "\tvariable/sum2:\t%1.31f\n", db_sum_assigned);
		fprintf(stderr, "\tvariable/mul:\t%1.31f\n", db_product);
		fprintf(stderr, "\n");
	}

	{
		double db;
		db = 0.1f; /* -f suffix */

		double db_sum;
		db_sum = db + db + db + db + db + db + db + db + db + db;

		double db_product;
		db_product = 10 * db;

		fprintf(stderr, "expected:\t\t%1.31f\n", 1.0);
		fprintf(stderr, "\timmediate/mul:\t%1.31f (-f suffix)\n", (db * 10));
		fprintf(stderr, "\tvariable/sum:\t%1.31f\n", db_sum);
		fprintf(stderr, "\tvariable/mul:\t%1.31f\n", db_product);
		fprintf(stderr, "\n");
	}

	{
		double db;
		db = 0.1f; /* initialized via -l suffix */

		double db_sum;
		db_sum = db + db + db + db + db + db + db + db + db + db;

		double db_product;
		db_product = 10 * db;

		fprintf(stderr, "expected:\t\t%1.31f\n", 1.0);
		fprintf(stderr, "\timmediate/mul:\t%1.31f (-l suffix)\n", (db * 10));
		fprintf(stderr, "\tvariable/sum:\t%1.31f\n", db_sum);
		fprintf(stderr, "\tvariable/mul:\t%1.31f\n", db_product);
		fprintf(stderr, "\n");
	}

	{
		double db;
		db = 0.1L; /* initialized via -L suffix */

		double db_sum;
		db_sum = db + db + db + db + db + db + db + db + db + db;

		double db_product;
		db_product = 10 * db;

		fprintf(stderr, "expected:\t\t%1.31f\n", 1.0);
		fprintf(stderr, "\timmediate/mul:\t%1.31f (-L suffix)\n", (db * 10));
		fprintf(stderr, "\tvariable/sum:\t%1.31f\n", db_sum);
		fprintf(stderr, "\tvariable/mul:\t%1.31f\n", db_product);
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "READY.\n");
	return 0;
}
