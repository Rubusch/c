/*
 the c function, called fro ASM
*/
#include <stdio.h>

extern int print_result(double result);

int print_result(double result) {
    printf("Circle radius is - %f\n", result);
    return 0;
}
