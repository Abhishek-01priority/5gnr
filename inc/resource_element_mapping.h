#ifndef RESOURCE_ELEMENT_MAPPING_H
#define RESOURCE_ELEMENT_MAPPING_H

#include "common.h"
#include "evm_lut.h"
#include "mem_alloc.h"
#include "bit_and_symbol.h"
#include "testcaseparamsgen.h"
#include "resource_element_mapping.h"

typedef enum { DMRS_COMB_2 = 2, DMRS_COMB_3 = 3 } dmrs_comb_t;

typedef struct {             
    uint16_t *idx; // PRB indices
    uint16_t  n;   // number of PRBs

} prb_vec_t; // variable-length PRB list for a symbol

typedef struct {
    uint16_t *idx; // RE indices
    uint16_t n;    // number of REs

} re_vec_t; // variable-length RE list for a symbol

// Per-slot (not per-symbol) masks saying which OFDM symbols are active
typedef struct {
    uint16_t pdcch_symmask;       // 14-bit, bit s=1 → PDCCH present on symbol s
    uint16_t dmrs_symmask_ue[3];        // PDSCH-DMRS symbol positions
    uint16_t pdsch_symmask_ue[3]; // for your 3 UEs (0..2). Generalize later.

} symplan_t;

typedef struct {
    dmrs_comb_t comb;         // 2 or 3
    uint8_t     delta_shift;  // 0..2
    uint8_t     type;         // 1 or 2
    uint8_t     add_pos;      // 0..3 (additionalPosition)

} dmrs_cfg_t;

typedef struct {
    prb_vec_t   pdsch_prbs_ue[3][14]; // PRB indices, for non-dmrs symbols only
    re_vec_t    pdschdmrsue[3][14]; // RE indices for PDSCH-dmrs. Used in symbols containing DMRS
    re_vec_t    pdschdataue[3][14]; // RE indices for PDSCH-data. Used in symbols containing DMRS
} pdsch_params_t;

typedef struct {
    re_vec_t    pdcchdata;
    re_vec_t    pdcchdmrs;

} pdcch_params_t;

typedef struct {

    uint16_t    nfft;

    symplan_t   plan;                // per-slot symbol plans
    dmrs_cfg_t  dmrs;                // DMRS parameters

    pdcch_params_t pdcchparams;
    pdsch_params_t pdschparams;

} rem_metadata_t;

typedef enum {
    PING = 0, PONG = 1
} buftype_t;

/** 
 * @brief       Fill per symbol subcarrier indices for PDSCH, PDCCH and DMRS. This function should be called only once in beginning
 * @param       nrb to calculate the total number of subcarriers
 * @return      rem_metadata_t
 */
void rem_metadata_gen(rem_metadata_t* rem_metadata, uint32_t nrb, nr_scs_khz_t scs, int bw);

void resource_element_mapping(rem_metadata_t* rem_metadata, uint8_t symidx, complex_t *buf, channelsym_t* channelsym);



#endif