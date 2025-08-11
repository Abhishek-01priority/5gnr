#include "numerical.h"
#include "testcaseparamsgen.h"

#define NA (-1) //TODO: move to main header
#define BW_IDX (bw <= 50 ? bw/5-1 : bw/10+4)

/* Transmission bandwidth configuration for FR1 taken from 38.104 v17.8.0 table 5.3.2-1 */
static const int transmission_bandwidth_LUT[3][15] = {
    /*5MHz  10MHz   15MHz   20MHz   25MHz   30MHz   35MHz   40MHz   45MHz   50MHz   60MHz   70MHz   80MHz   90MHz   100MHz*/
    {25,    52,     79,     106,    133,    160,    188,    216,    242,    270,    NA,     NA,     NA,     NA,     NA}, 
    {11,    24,     38,     51,     65,     78,     92,     106,    119,    133,    162,    189,    217,    245,    273},
    {NA,    11,     18,     24,     31,     38,     44,     51,     58,     65,     79,     93,     107,    121,    135},
};

/* Nominal RBG size P Table 5.1.2.1-1 38.214 v17.1.0*/
static const int P[4][4] = {
    /*Low BWP   High BWP    Configuration1  Configuration2*/
    {1,         36,         2,              4},
    {37,        72,         4,              8},
    {73,        144,        8,              16},
    {145,       275,        16,             16},
};

int fetch_nrbs(int scs/*in KHz*/, int bw /*in MHz*/) {

    int nrb;

    switch(scs) {
        case 15:
            nrb = transmission_bandwidth_LUT[0][BW_IDX];
            break;
        case 30:
            nrb = transmission_bandwidth_LUT[1][BW_IDX];
            break;
        case 60:
            nrb = transmission_bandwidth_LUT[2][BW_IDX];
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

int NR_FR1_TM3_2(int nrb, NR_FR1_TM32_t* nrfr1tm32) {
    /* target percentage of 16 QAM PDSCH PRBs deboosted */
    float x = 0.6;
    int level_of_deboosting = -3;

    int nsizeBWP = nrb - 3, p;
    if (nsizeBWP >= P[0][0] && nsizeBWP <= P[0][1]) { p = P[0][3]; }
    else if (nsizeBWP >= P[1][0] && nsizeBWP <= P[1][1]) { p = P[1][3]; }
    else if (nsizeBWP >= P[2][0] && nsizeBWP <= P[2][1]) {p = P[2][3]; }
    else if (nsizeBWP >= P[3][0] && nsizeBWP <= P[3][1]) {p = P[3][3]; }
    else {return -1; /*invalid request*/}

    /*# of 16QAM PDSCH RBGs within a slot for which EVM is measured */
    int nrbg_part1 = x * nsizeBWP / p;
    int nrbg_part2 = 0.5 * ( (nsizeBWP + 3%p - p) / p  - ((nsizeBWP + 3%p - p) / p)%2 ) + 1;
    int nrbg = nrbg_part1 < nrbg_part2 ? nrbg_part1 : nrbg_part2; // choose minimum

    int last_deboost_16qam_location, i = 2 * (nrbg - 2) + 1, j = 0;
    /* Locations of 16QAM RBGs which are deboosted */
    last_deboost_16qam_location = (nrb - 3 + 3%p) / p - 1;
    int* deboosted_16qam_locs = malloc(sizeof(int) * last_deboost_16qam_location);
    while ( i > 0 ) {
        deboosted_16qam_locs[j] = last_deboost_16qam_location - i;
        i = i - 2;
        j++;
    }
    if (j >= nrbg) { assert(0); }
    deboosted_16qam_locs[j] = last_deboost_16qam_location;

    /* converting RBG to PRB bitmap 
    For this testcase: 16-QAM and QPSK is used. To differentiate the PRBs that are 16-QAM modulated and QPSK modulated
    if bit is set to 1, the modulation is 16-QAM and if the bit is 0, modulation is QPSK. 
    By default all PRBs are initialized with QPSK */
    uint64_t pdsch_bitmap_16qam = 0x0;
    pdsch_bitmap_16qam = rbg_to_prb_bitmap_nrfr1tm32(deboosted_16qam_locs, nrbg);


    /* # of QPSK PDSCH PRBs within a slot for which EVM is not measured (used for power balancing) */
    // int qpsk_pdsch_prbs_notusedforEVM = nrb - 3 - p * nrbg;

    /* Locations of PDSCH RBGs which are boosted: Starting at RB#3 and excluding PRBs of RBGs which are deboosted */
    uint64_t boosted_prb_pdsch_bitmap = 0x0; // 0 -> boosted, 1 -> deboosted
    /* since all 16-QAM prbs are deboosted, we can use `pdsch_bitmap` as a reference. 
       Also boosting has to start from rb idx 3 */
    boosted_prb_pdsch_bitmap = pdsch_bitmap_16qam;

    /* level of boosting in Q16.16 format */
    int level_of_boosting = levelofboosting(nrb, p, nrbg);

    nrfr1tm32->level_of_boosting            = level_of_boosting;
    nrfr1tm32->level_of_deboosting          = level_of_deboosting;
    nrfr1tm32->pdsch_mod_and_boost_bitmap   = boosted_prb_pdsch_bitmap;

    return 0;
    
}