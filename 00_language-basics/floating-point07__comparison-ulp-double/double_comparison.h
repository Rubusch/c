#ifndef DOUBLE_COMPARISON_H
#define DOUBLE_COMPARISON_H


typedef union double_wrap_s
{
    int64_t int_repr;
    double dbl;
#ifdef _DEBUG
    struct {
        uint64_t mantissa : 52;
        uint64_t exponent : 11;
        uint64_t sign : 1;
    } dbg_parts;
#endif
} double_wrap_t;
typedef double_wrap_t* double_wrap_p;

bool negative(double_wrap_p fp);
int64_t raw_mantissa(double_wrap_p fp);
int64_t raw_exponent(double_wrap_p fp);

bool relatively_equal(double a, double b, int max_ulp_diff);


#endif /* DOUBLE_COMPARISON_H */
