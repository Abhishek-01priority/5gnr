#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include "common.h"
#include "kiss_fft.h"

static inline int16_t sat16(int32_t x)
{
    if (x >  32767) return  32767;
    if (x < -32768) return -32768;
    return (int16_t)x;
}

/* Wrapper: out = FFT(in); is_inverse!=0 does IFFT. nfft must be a valid KissFFT size. */
void fft_q8_8(complex_t *out, const complex_t *in, int is_inverse, int nfft, uint32_t numREpersym)
{
    assert(out && in && (nfft > 0));

    /* Allocate KissFFT config (internal twiddles, etc.) */
    size_t tmp_bytes = 0;
    kiss_fft_cfg cfg = kiss_fft_alloc(nfft, is_inverse, NULL, &tmp_bytes);
    assert(cfg);

    /* I/O buffers in KissFFT format (Q8.8 -> int16_t) */
    kiss_fft_cpx *xin  = (kiss_fft_cpx*)malloc((size_t)nfft * sizeof(kiss_fft_cpx));
    kiss_fft_cpx *xout = (kiss_fft_cpx*)malloc((size_t)nfft * sizeof(kiss_fft_cpx));
    assert(xin && xout);
    memset(xin , 0, (size_t)nfft * sizeof(kiss_fft_cpx));
    memset(xout, 0, (size_t)nfft * sizeof(kiss_fft_cpx));

    /* Copy/convert input: complex_t(Q8.8) -> kiss_fft_cpx(int16) */
    for (uint32_t i = 0; i < numREpersym; ++i) {
        /* If complex_t already stores Q8.8 in int16, this is just a cast+saturate. */
        xin[i].r = in[i].i; // real
        xin[i].i = in[i].q; // imag
    }

    /* Execute FFT/IFFT */
    kiss_fft(cfg, xin, xout);

    /* Copy back to caller buffer (still Q8.8 int16). 
       NOTE on scaling:
       - In fixed-point builds, KissFFT internally manages headroom. By default,
         the inverse FFT applies 1/N scaling; the forward does not. If you defined
         KISS_FFT_NOSCALE or changed defaults, adjust scaling here as needed. */
    for (int i = 0; i < nfft; ++i) {
        out[i].i = sat16((int)xout[i].r);
        out[i].q = sat16((int)xout[i].i);
    }

    /* Cleanup */
    free(xin);
    free(xout);
    free(cfg); /* OK whether cfg was individually malloc'ed or points into work buffer */
}
