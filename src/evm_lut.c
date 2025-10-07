// 3GPP TS 38.141-1 v17.8.0
// EVM window tables for FR1 normal CP: 15/30/60 kHz SCS (Tables 6.5.3.5-2, -3, -4)

#include "evm_lut.h"

// ---- Table 6.5.3.5-2: FR1, 15 kHz SCS ----
// cp_len applies to symbols 1–6 and 8–13 (symbols 0 & 7 have longer CP; lower percentage)
static const nr_evm_entry_t EVM_FR1_15KHZ[] = {
    {  5,  512,  36,  14, 40 },
    { 10, 1024,  72,  28, 40 },
    { 15, 1536, 108,  44, 40 },
    { 20, 2048, 144,  58, 40 },
    { 25, 2048, 144,  72, 50 },
    { 30, 3072, 216, 108, 50 },
    { 35, 3072, 216, 108, 50 },
    { 40, 4096, 288, 144, 50 },
    { 45, 4096, 288, 144, 50 },
    { 50, 4096, 288, 144, 50 },
};

// ---- Table 6.5.3.5-3: FR1, 30 kHz SCS ----
// cp_len applies to symbols 1–13 (symbol 0 has longer CP; lower percentage)
static const nr_evm_entry_t EVM_FR1_30KHZ[] = {
    {   5,  256,  18,   8, 40 },
    {  10,  512,  36,  14, 40 },
    {  15,  768,  54,  22, 40 },
    {  20, 1024,  72,  28, 40 },
    {  25, 1024,  72,  36, 50 },
    {  30, 1536, 108,  54, 50 },
    {  35, 1536, 108,  54, 50 },
    {  40, 2048, 144,  72, 50 },
    {  45, 2048, 144,  72, 50 },
    {  50, 2048, 144,  72, 50 },
    {  60, 3072, 216, 130, 60 },
    {  70, 3072, 216, 130, 60 },
    {  80, 4096, 288, 172, 60 },
    {  90, 4096, 288, 172, 60 },
    { 100, 4096, 288, 172, 60 },
};

// ---- Table 6.5.3.5-4: FR1, 60 kHz SCS ----
// cp_len applies to all OFDM symbols except symbol 0 of slot 0 & slot 2 (longer CP; lower percentage)
static const nr_evm_entry_t EVM_FR1_60KHZ[] = {
    {  10,  256,  18,   8, 40 },
    {  15,  384,  27,  11, 40 },
    {  20,  512,  36,  14, 40 },
    {  25,  512,  36,  18, 50 },
    {  30,  768,  54,  26, 50 },
    {  35,  768,  54,  26, 50 },
    {  40, 1024,  72,  36, 50 },
    {  45, 1024,  72,  36, 50 },
    {  50, 1024,  72,  36, 50 },
    {  60, 1536, 108,  64, 60 },
    {  70, 1536, 108,  64, 60 },
    {  80, 2048, 144,  86, 60 },
    {  90, 2048, 144,  86, 60 },
    { 100, 2048, 144,  86, 60 },
};

// Generic lookup
const nr_evm_entry_t* nr_evm_lookup(nr_scs_khz_t scs, int bw_mhz) {
    const nr_evm_entry_t *tbl = NULL;
    size_t N = 0;

    switch (scs) {
        case NR_SCS_15: tbl = EVM_FR1_15KHZ; N = sizeof(EVM_FR1_15KHZ)/sizeof(tbl[0]); break;
        case NR_SCS_30: tbl = EVM_FR1_30KHZ; N = sizeof(EVM_FR1_30KHZ)/sizeof(tbl[0]); break;
        case NR_SCS_60: tbl = EVM_FR1_60KHZ; N = sizeof(EVM_FR1_60KHZ)/sizeof(tbl[0]); break;
        default: return NULL;
    }
    for (size_t i = 0; i < N; ++i) if (tbl[i].bw_mhz == bw_mhz) return &tbl[i];
    return NULL; // not found
}

// Helper: Fs (Hz) = SCS (Hz) * Nfft
static inline double nr_sampling_rate_hz(nr_scs_khz_t scs, int nfft) {
    return (double)scs * 1e3 * (double)nfft;
}