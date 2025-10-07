#include "common.h"

typedef struct {
    int bw_mhz;        // Channel bandwidth (MHz)
    int nfft;          // FFT size
    int cp_len;        // CP length in FFT samples (see table notes)
    int evm_win_W;     // EVM window length W (samples)
    int ratio_pct;     // Ratio W / CP (%)
} nr_evm_entry_t;

const nr_evm_entry_t* nr_evm_lookup(nr_scs_khz_t scs, int bw_mhz);