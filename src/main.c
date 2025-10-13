/* \brief: Simulate EVM measurement in presence of RF impairements */
#include "misc.h"
#include "common.h"
#include "mem_alloc.h"
#include "modulation.h"
#include "bit_and_symbol.h"
#include "resource_element_mapping.h"

void fft_q8_8(complex_t *out, const complex_t *in, int is_inverse, int nfft, uint32_t numREpersym);

#define NUM_SC_PER_RB (12)
#define __DEBUG__

int main() {
    /*
     * Procedure (per Figure H.2.4-1):
     *   1) Bits generation
     *   2) Modulation mapping
     *   3) Resource element mapping
     *   4) OFDM modulation
     *   5) Pre-FFT frequency and time correction
     *   6) CP removal
    */
    // system configs
    nr_scs_khz_t scs = NR_SCS_30;
    nr_bandwidth_t bw = BW_10_MHZ;
    uint32_t nrb = fetch_nrbs(scs, bw);

    // Generating bits
    bits_t* bits = mem_alloc(sizeof(bits_t));
    memset(bits, 0, sizeof(bits_t));
    bitgen(bits);

    // Generating symbols
    channelsym_t* channelsym = mem_alloc(sizeof(channelsym_t));
    memset(channelsym, 0, sizeof(channelsym_t));
    symbolgen(bits, channelsym);

    // Resource Element mapping metadata generation (should happen only once)
    rem_metadata_t* rem_metadata = malloc(sizeof(rem_metadata_t));
    rem_metadata_gen(rem_metadata, nrb,  scs, bw);
    
    // Do REM and IFFT 1 OFDM symbol at a time
    uint32_t nfft = 1024;
    uint32_t numREpersym = nrb * NUM_SC_PER_RB;
    assert(numREpersym <= nfft);
    complex_t *re = mem_alloc(sizeof(complex_t) * numREpersym); // for 1 OFDM symbol only
    complex_t *ifft_samples = mem_alloc(sizeof(complex_t) * nfft);
    for (uint8_t symidx = 0; symidx < 14; symidx++) {
        resource_element_mapping(rem_metadata, symidx, re, channelsym);
        fft_q8_8(ifft_samples, re, 0, nfft, numREpersym);
        #ifdef __DEBUG__
        write_symbols_to_file("debug\\ifft_samples.bin", ifft_samples, nfft);
        #endif
        exit(0);
    }

    return 0;
}
