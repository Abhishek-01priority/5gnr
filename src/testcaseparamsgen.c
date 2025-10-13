#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "numerical.h"
#include "testcaseparamsgen.h"

// https://in.mathworks.com/help/5g/ug/5g-nr-tm-waveform-generation.html

#define NA (0) //TODO: move to main header
#define BW_IDX (bw <= 50 ? bw/5-1 : bw/10+4)

/* Transmission bandwidth configuration for FR1 taken from 38.104 v17.8.0 table 5.3.2-1 */
static const int transmission_bandwidth_LUT[3][15] = {
            /*5MHz  10MHz   15MHz   20MHz   25MHz   30MHz   35MHz   40MHz   45MHz   50MHz   60MHz   70MHz   80MHz   90MHz   100MHz*/
/*15KHz*/    {25,    52,     79,     106,    133,    160,    188,    216,    242,    270,    NA,     NA,     NA,     NA,     NA}, 
/*30KHz*/    {11,    24,     38,     51,     65,     78,     92,     106,    119,    133,    162,    189,    217,    245,    273},
/*60KHz*/    {NA,    11,     18,     24,     31,     38,     44,     51,     58,     65,     79,     93,     107,    121,    135},
};

/* Nominal RBG size P Table 5.1.2.1-1 38.214 v17.1.0*/
static const int P[4][4] = {
    /*Low BWP   High BWP    Configuration1  Configuration2*/
    {1,         36,         2,              4},
    {37,        72,         4,              8},
    {73,        144,        8,              16},
    {145,       275,        16,             16},
};

uint32_t fetch_nrbs(nr_scs_khz_t scs, nr_bandwidth_t bw) {

    uint32_t nrb;

    switch(scs) {
        case NR_SCS_15:
            nrb = transmission_bandwidth_LUT[0][bw];
            break;
        case NR_SCS_30:
            nrb = transmission_bandwidth_LUT[1][bw];
            break;
        case NR_SCS_60:
            nrb = transmission_bandwidth_LUT[2][bw];
            break;
        default:
            nrb = NA;
    }

    return nrb;
}

uint64_t rbg_to_prb_bitmap_nrfr1tm32(int *input_rbg, int num_nrbg) {
    /* input_rbg is an array of rbg indices starting from 0. For any rbg index 'n', the corresponding prbs are '2n, 2n+1'. 
    The prbs in bitmap is read right to left, i.e., right most bit -> prb idx 0 and left most bit -> prb idx 63 */
    uint64_t pdsch_bitmap = 0x0;
    for (int i = 0; i < num_nrbg; i++) {
        pdsch_bitmap = pdsch_bitmap | (1 << ( 2 * input_rbg[i] ) );
        pdsch_bitmap = pdsch_bitmap | (1 << ( 2 * input_rbg[i] + 1 ) );
    }

    return pdsch_bitmap;
}

// Helper to improve readability
static inline uint16_t min(uint16_t a, uint16_t b) {
    return a < b ? a : b;
}

uint16_t * rbgarray_to_prbarray(const uint16_t *rbg, size_t nrbg,
                         uint16_t nrb, uint8_t P,
                         size_t *out_n, uint64_t *prbbitmap) {
    // 1. Initial validation
    if (!rbg || !out_n || prbbitmap == 0 || P == 0 || nrb == 0) { assert(0); }
    if (out_n) { *out_n = 0; } // Ensure output size is always initialized
    if (prbbitmap) { *prbbitmap = 0; }

    // This is equivalent to ceil(nrb / P) and represents the first invalid index.
    const uint16_t num_rbgs_in_grid = (nrb + P - 1) / P;

    // 2. Pass 1: Count PRBs to allocate exactly
    size_t prb_count = 0;
    for (size_t i = 0; i < nrbg; ++i) {
        const uint16_t g = rbg[i];
        if (g < num_rbgs_in_grid) { // Skip out-of-range RBGs
            const uint16_t start_prb = g * P;
            // The last RBG might be smaller than P
            const uint16_t end_prb = min(start_prb + P, nrb);
            prb_count += (end_prb - start_prb);
        }
    }
    assert(prb_count);
    assert(prb_count <= (SIZE_MAX / sizeof(uint16_t))); // Guard against overflow for malloc

    // 3. Allocate memory
    uint16_t *prb = (uint16_t *)mem_alloc(prb_count * sizeof(uint16_t));
    assert(prb);

    // 4. Pass 2: Fill the array
    uint16_t *writer = prb; // Use a pointer for filling. prb[i++] can be used as well, but I like this
    for (size_t i = 0; i < nrbg; ++i) {
        const uint16_t g = rbg[i];
        if (g < num_rbgs_in_grid) { // Same logic as pass 1
            const uint16_t start_prb = g * P;
            const uint16_t end_prb = start_prb + P < nrb ? start_prb + P : nrb;
            for (uint16_t p = start_prb; p < end_prb; ++p) {
                *writer++ = p;
                *prbbitmap |= (1 << p);
            }
        }
    }

    *out_n = prb_count;
    return prb;
}

int NR_FR1_TM3_2(uint32_t nrb, NR_FR1_TM32_t* nrfr1tm32) {
    /* target percentage of 16 QAM PDSCH PRBs deboosted */
    float x = 0.6;
    int level_of_deboosting = -3;

    int nsizeBWP = nrb - 3, p;
    if (nsizeBWP >= P[0][0] && nsizeBWP <= P[0][1]) { p = P[0][3]; }
    else if (nsizeBWP >= P[1][0] && nsizeBWP <= P[1][1]) { p = P[1][3]; }
    else if (nsizeBWP >= P[2][0] && nsizeBWP <= P[2][1]) {p = P[2][3]; }
    else if (nsizeBWP >= P[3][0] && nsizeBWP <= P[3][1]) {p = P[3][3]; }
    else {return -1; /*invalid request*/}

    /* number of 16QAM PDSCH RBGs within a slot for which EVM is measured */
    uint32_t nrbg_part1 = x * nsizeBWP / p;
    uint32_t nrbg_part2 = 0.5 * ( (nsizeBWP + 3%p - p) / p  - ((nsizeBWP + 3%p - p) / p)%2 ) + 1;
    uint32_t nrbg = nrbg_part1 < nrbg_part2 ? nrbg_part1 : nrbg_part2; // choose minimum

    uint16_t last_deboost_16qam_location; // in terms of RBG (and not PRB)
    uint16_t i = 2 * (nrbg - 2) + 1, j = 0;
    /* Locations of 16QAM RBGs which are deboosted */
    last_deboost_16qam_location = (nrb - 3 + 3%p) / p - 1;
    uint16_t* deboosted_16qam_locs = mem_alloc(sizeof(uint16_t) * nrbg);
    while ( i > 0 ) {
        deboosted_16qam_locs[j] = last_deboost_16qam_location - i;
        i = i - 2;
        j++;
    }
    assert(j < nrbg);
    deboosted_16qam_locs[j] = last_deboost_16qam_location;

    /*converting RBGs to PRBs*/
    uint64_t rnti1prbbitmap = 0;
    nrfr1tm32->pdschprb[1].idx = rbgarray_to_prbarray(deboosted_16qam_locs, 
        nrbg, nrb, p, &nrfr1tm32->pdschprb[1].n, &rnti1prbbitmap);
    // the rbgs are of no use now and can be freed
    mem_free(deboosted_16qam_locs);
    assert(nrfr1tm32->pdschprb[1].n < 64); // added this because rnti1prbbitmap is set to 64 bits for now

    /**Boosted PDSCH PRBs for RNTI 0. All PRBs except the deboosted 16 QAM prbs are boosted QPSK prbs for RNTI 0.
     * Iterate throught the list and find the PRBs that are not allocated to 16 QAM.
     * Also RNTI0 and RNTI1 allocation starts from NRB #3 */
    nrfr1tm32->pdschprb[0].n =  nrb - 3 - p * nrbg; // Table 4.9.2.2.7-1
    nrfr1tm32->pdschprb[0].idx = mem_alloc(sizeof(uint16_t) * nrfr1tm32->pdschprb[0].n);
    uint16_t *writer = nrfr1tm32->pdschprb[0].idx;
    for (uint16_t prb = 3; prb < nrb; prb++) { //prb numbering starts from 0
        if ( ! ( (rnti1prbbitmap >> prb) & 0x1 ) ) {
            *writer++ = prb;
        }
    }

    nrfr1tm32->pdschprb[2].n = 3; // PRB0,1,2
    nrfr1tm32->pdschprb[2].idx = mem_alloc(sizeof(uint16_t) * nrfr1tm32->pdschprb[2].n);
    for (uint16_t prb = 0; prb < nrfr1tm32->pdschprb[2].n; prb++) {
        nrfr1tm32->pdschprb[2].idx[prb] = prb;
    }

    /* level of boosting in Q16.16 format */
    uint32_t level_of_boosting = levelofboosting(nrb, p, nrbg);

    nrfr1tm32->level_of_boosting = level_of_boosting; // rnti 0
    nrfr1tm32->level_of_deboosting = level_of_deboosting; // rnti 1

    /* PDCCH */
    // assuming the pdcch allocations remains constant accross symbols, which is the case for NR-FR1-TM3.2
    // assuming PDCCH has been given contiguous allocation
    // This can be moved to a common function since it is constant across different test models for DL EVM
    nrfr1tm32->pdcchprb.n = 3;
    nrfr1tm32->pdcchprb.idx = mem_alloc(sizeof(uint16_t) * nrfr1tm32->pdcchprb.n); assert(nrfr1tm32->pdcchprb.idx);
    for (uint8_t prb = 0; prb < nrfr1tm32->pdcchprb.n; prb++) {
        nrfr1tm32->pdcchprb.idx[prb] = prb; 
    }

    return 0;
    
}