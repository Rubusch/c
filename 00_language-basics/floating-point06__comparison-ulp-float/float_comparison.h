#ifndef FLOAT_COMPARISON_H
#define FLOAT_COMPARISON_H


typedef union float_wrap_s
{
    int32_t int_repr;
    float flo;
#ifdef _DEBUG
    struct {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    } dbg_parts;
#endif
} float_wrap_t;
typedef float_wrap_t* float_wrap_p;

bool negative(float_wrap_p flo);
int32_t raw_mantissa(float_wrap_p flo);
int32_t raw_exponent(float_wrap_p flo);

bool relatively_equal(double a, double b, int max_ulp_diff);


#endif /* FLOAT_COMPARISON_H */
