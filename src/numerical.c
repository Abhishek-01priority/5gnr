#include "numerical.h"

#define Q (16) // follow Q16.16 format
#define ONE_Q (1 << Q)
#define LUT_BITS (8)
#define FRAC_SHIFT (Q - LUT_BITS)
#define TEN_DIVIDEDBY_LOG2OF10 (3.0102999566398121)

// log2(1 + f) where f = [0:255], floor(log2(1+[0:255]/256) * 2^16 + 0.5)
const uint32_t LUT[256] = {
0x0, 0x171, 0x2e0, 0x44e, 0x5ba, 0x725, 0x88e, 0x9f7, 
0xb5d, 0xcc3, 0xe27, 0xf8a, 0x10eb, 0x124b, 0x13aa, 0x1508, 
0x1664, 0x17bf, 0x1919, 0x1a71, 0x1bc8, 0x1d1e, 0x1e73, 0x1fc6, 
0x2119, 0x226a, 0x23ba, 0x2508, 0x2656, 0x27a2, 0x28ed, 0x2a37, 
0x2b80, 0x2cc8, 0x2e0f, 0x2f54, 0x3098, 0x31dc, 0x331e, 0x345f, 
0x359f, 0x36de, 0x381b, 0x3958, 0x3a94, 0x3bce, 0x3d08, 0x3e41, 
0x3f78, 0x40af, 0x41e4, 0x4319, 0x444c, 0x457f, 0x46b0, 0x47e1, 
0x4910, 0x4a3f, 0x4b6c, 0x4c99, 0x4dc5, 0x4eef, 0x5019, 0x5142, 
0x526a, 0x5391, 0x54b7, 0x55dc, 0x5700, 0x5824, 0x5946, 0x5a68, 
0x5b89, 0x5ca8, 0x5dc7, 0x5ee5, 0x6003, 0x611f, 0x623a, 0x6355, 
0x646f, 0x6588, 0x66a0, 0x67b7, 0x68ce, 0x69e4, 0x6af8, 0x6c0c, 
0x6d20, 0x6e32, 0x6f44, 0x7055, 0x7165, 0x7274, 0x7383, 0x7490, 
0x759d, 0x76aa, 0x77b5, 0x78c0, 0x79ca, 0x7ad3, 0x7bdb, 0x7ce3, 
0x7dea, 0x7ef0, 0x7ff6, 0x80fb, 0x81ff, 0x8302, 0x8405, 0x8507, 
0x8608, 0x8709, 0x8809, 0x8908, 0x8a06, 0x8b04, 0x8c01, 0x8cfe, 
0x8dfa, 0x8ef5, 0x8fef, 0x90e9, 0x91e2, 0x92db, 0x93d2, 0x94ca, 
0x95c0, 0x96b6, 0x97ab, 0x98a0, 0x9994, 0x9a87, 0x9b7a, 0x9c6c, 
0x9d5e, 0x9e4f, 0x9f3f, 0xa02e, 0xa11e, 0xa20c, 0xa2fa, 0xa3e7, 
0xa4d4, 0xa5c0, 0xa6ab, 0xa796, 0xa881, 0xa96a, 0xaa53, 0xab3c, 
0xac24, 0xad0c, 0xadf2, 0xaed9, 0xafbe, 0xb0a4, 0xb188, 0xb26c, 
0xb350, 0xb433, 0xb515, 0xb5f7, 0xb6d9, 0xb7ba, 0xb89a, 0xb97a, 
0xba59, 0xbb38, 0xbc16, 0xbcf4, 0xbdd1, 0xbead, 0xbf8a, 0xc065, 
0xc140, 0xc21b, 0xc2f5, 0xc3cf, 0xc4a8, 0xc580, 0xc658, 0xc730, 
0xc807, 0xc8de, 0xc9b4, 0xca8a, 0xcb5f, 0xcc34, 0xcd08, 0xcddc, 
0xceaf, 0xcf82, 0xd054, 0xd126, 0xd1f7, 0xd2c8, 0xd399, 0xd469, 
0xd538, 0xd607, 0xd6d6, 0xd7a4, 0xd872, 0xd93f, 0xda0c, 0xdad9, 
0xdba5, 0xdc70, 0xdd3b, 0xde06, 0xded0, 0xdf9a, 0xe063, 0xe12c, 
0xe1f5, 0xe2bd, 0xe385, 0xe44c, 0xe513, 0xe5d9, 0xe69f, 0xe765, 
0xe82a, 0xe8ef, 0xe9b3, 0xea77, 0xeb3b, 0xebfe, 0xecc1, 0xed83, 
0xee45, 0xef06, 0xefc8, 0xf088, 0xf149, 0xf209, 0xf2c8, 0xf387, 
0xf446, 0xf505, 0xf5c3, 0xf680, 0xf73e, 0xf7fb, 0xf8b7, 0xf973, 
0xfa2f, 0xfaea, 0xfba5, 0xfc60, 0xfd1a, 0xfdd4, 0xfe8e, 0xff47
};

uint32_t fixed_mul(uint32_t op1, uint32_t op2) { return ((uint64_t)op1 * op2 >> Q);  /*convert to 64 bit to avoid overflow and right shift */}
// note: when doing multiplication typecast the first operand to 64 bit and then multiply. This causes the multiplication to of 64 bit
// (uint64_t)(a * b) will typecast the result of a*b to 64 bits but by then higher bits are already lost.
// same applies for div() function, typecast op1 before left shifting because op1 is already 32 bit.
uint32_t fixed_div(uint32_t op1, uint32_t op2) { return ( ((uint64_t)op1 << Q) / op2 ); /*multiply numerator by 2^Q to ensure precision*/}

// Portable CLZ fallback (use compiler builtin if you have it)
static inline int clz_32(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_clz(x);
#else
    // Simple portable version (not the fastest)
    int n = 0;
    if (x == 0) return 32;
    while ((x & 0x80000000u) == 0) { x <<= 1; ++n; }
    return n;
#endif
}

uint32_t logof2(uint32_t x) {

    // write `x` as m*2^n, so that log2(x) = n + log2(m). n is the integer part and log2(m) is the frac of log2(x).
    // To find `n`, count leading zeros of x. Since `x` is a fixed point number, the real number is x/2^16, so subtract 16 from clz()
    int n = 31 - clz_32(x) - 16;

    // now, `m` = x/2^n and handle the scenario of `x` being purely fractional. Now `m` is in [1,2)
    uint32_t m;
    if ( n >= 0 ) {
        m = x >> n;
    }
    else {
        m = x << (-n);
    }

    // convert m = 1 + f, so that `f` is in [0,1)
    uint32_t onedot0 = (uint32_t)ONE_Q;
    uint32_t frac = m - onedot0;

    // find the index for LUT. frac holds 16 bits with msb 8 bits as bucket idx
    uint32_t bucket_idx = frac >> FRAC_SHIFT;

    // since LUT is generated for 256 values, the exact value of log2(m) may not be in LUT and interpolation needs to done
    // check if exact value of log2(m) lies in LUT or not by doing frac % 256
    uint32_t rem = frac & 0xFF;

    uint32_t low, high, dy, y;
    if (rem == 0) {
        y  = LUT[bucket_idx];
    }
    else {
        low = LUT[bucket_idx];
        high = LUT[bucket_idx + 1];
        dy = high - low;
        y =  low + (uint32_t)( ( ((uint64_t)dy * rem) >> FRAC_SHIFT) ); // 2 point interpolation: y = y_low + dy * rem
    }

    return (uint32_t)( (n << Q) | y );

}

int levelofboosting(uint32_t nrb, uint32_t P, uint32_t nrbg) {

    const uint32_t ten_power_minus3_by_10 = (uint32_t)( (0.5011872336272722 * ONE_Q) + 0.5 );
    uint32_t ten_power_minus3_by_10_with_PNrbg = fixed_mul(ten_power_minus3_by_10, (P*nrbg) << Q);
    uint32_t nrb_minus_three = (nrb - 3) << Q;
    uint32_t numerator = (nrb_minus_three - ten_power_minus3_by_10_with_PNrbg);
    uint32_t denominator = (nrb_minus_three - ((P*nrbg) << Q) );
    uint32_t ratio_q = fixed_div(numerator, denominator);

    // 10*log10(x) = 10*log2(x) / log2(10) = TEN_DIVIDEDBY_LOG2OF10 * log2(x)
    uint32_t log_2 = logof2(ratio_q);
    uint32_t val = fixed_mul((uint32_t)( TEN_DIVIDEDBY_LOG2OF10 * ONE_Q + 0.5 ), log_2);

    // printf("10log10(%0.6f) = %0.6f\n", (double)ratio_q/(double)ONE_Q, (double)val/(double)ONE_Q);

    return val;
}