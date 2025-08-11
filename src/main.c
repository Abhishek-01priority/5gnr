/* \brief: Simulate EVM measurement in presence of RF impairements */
#include "misc.h"
#include "common.h"
#include "mem_alloc.h"
#include "modulation.h"

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

    testmodel_t testmodel = NR_TM_FR1_TM32;

    // Bits generation
    //   Different bits for RNTI 0, 1, and 2
    const char *fname = "data\\pdsch_rnti1_databits.bin";
    size_t      num_bits = 0;
    uint8_t    *rnti1_bits = read_bits_from_file(fname, &num_bits);

    // Symbol mapping
    //   RNTI 1 -> 16-QAM, RNTI 0 -> QPSK, RNTI 2 -> QPSK
    //   16-QAM => 4 bits per symbol
    uint32_t   rnti1_numsymbols = (uint32_t)(num_bits >> 4);
    complex_t *rnti1_symarry    = mem_alloc(sizeof(complex_t) * rnti1_numsymbols);

    uint8_t nibble;
    for (uint32_t s = 0; s < rnti1_numsymbols; s++) {
        nibble = ((rnti1_bits[s * 4 + 0] << 3) | (rnti1_bits[s * 4 + 1] << 2) | (rnti1_bits[s * 4 + 2] << 1) | (rnti1_bits[s * 4 + 3] << 0));
        complex_t *sym = &rnti1_symarry[s];
        qammod_16(nibble, sym);
    }

#ifdef __DEBUG__
    write_symbols_to_file("debug\\rnti1_qam16_constellation.bin", rnti1_symarry, rnti1_numsymbols);
#endif

    return 0;
}
