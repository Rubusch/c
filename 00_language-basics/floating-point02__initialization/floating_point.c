/*
  floating point issues - real fun starts now!
*/

#include <stdio.h>
#include <float.h>

int main()
{
	fprintf(stderr, "float and double - be careful when comparing float and doubles!!!\n\n");

	float flo1;
	flo1 = 0.1f;

	float flo2;
	flo2 = 0.1;

	float flo3;
	flo3 = 0;

	float flo4;
	flo4 = 0.0;


	double dou1;
	dou1 = 0.1f;

	double dou2;
	dou2 = 0.1;

	double dou3;
	dou3 = 0;

	double dou4;
	dou4 = 0.0;

	fprintf(stderr, "float:\t'0.1f'\t= %1.15f,\n\t'0.1'\t= %1.15f,\n\t'0'\t= %1.15f,\n\t'0.0'\t= %1.15f\n", flo1, flo2, flo3, flo4);
	fprintf(stderr, "double:\t'0.1f'\t= %1.31f,\n\t'0.1'\t= %1.31f,\n\t'0'\t= %1.31f,\n\t'0.0'\t= %1.31f\n", dou1, dou2, dou3, dou4);

	fprintf(stderr, "READY.\n");
	return 0;
}
