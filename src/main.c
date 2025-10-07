/* \brief: Simulate EVM measurement in presence of RF impairements */
#include "misc.h"
#include "common.h"
#include "mem_alloc.h"
#include "modulation.h"
#include "bit_and_symbol.h"

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

    // Generating bits
    bits_t* bits = mem_alloc(sizeof(bits_t));
    memset(bits, 0, sizeof(bits_t));
    bitgen(bits);

    // Generating symbols
    channelsym_t* channelsym = mem_alloc(sizeof(channelsym_t));
    memset(channelsym, 0, sizeof(channelsym_t));
    symbolgen(bits, channelsym);

    // Resource Element mapping metadata generation (should happen only once)
    
    

    return 0;
}
