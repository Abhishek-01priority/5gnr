#include "resource_element_mapping.h"

#define NSCRB (12)
#define BOUND_CHECK(need, have, cursor) assert((cursor) + (need) <= (have))

// constant offsets inside a PRB for PDSCH DMRS REs
static const uint16_t PDSCH_DMRS_OFS[6] = {0, 2, 4, 6, 8, 10};
// constant offsets inside a PRB for PDSCH Data REs
static const uint16_t PDSCH_DATA_OFS[6] = {1, 3, 5, 7, 9, 11};

// constant offsets inside a PRB for PDCCH data and DMRS REs
static const uint16_t PDCCH_DATA_OFS[9] = {0,2,3,4,6,7,8,10,11};
static const uint16_t PDCCH_DMRS_OFS[3] = {1,5,9};

uint32_t pdcchdataREidx, pdcchdmrsREidx, pdschdmrsREidx[3], pdschdataREidx[3]; // counters for RE

    /* -- NR-TM-FR1-TM3.2 -- 
    SYM 0  | PDCCH | NRB 0 - 2 | TOTAL REs = 36 (27 data, 9 DMRS)   | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 1  | PDCCH | NRB 0 - 2 | TOTAL REs = 36 (27 data. 9 DMRS)   | PDSCH data (rnti = 0, 1)     | NRB 3 - END | 
    SYM 2  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (12 data, 12 DMRS)  | PDSCH data + DMRS (rnti 0,1) | NRB 3 - END |
    SYM 3  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 4  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 5  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 6  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 7  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 8  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 9  | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 10 | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 11 | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (12 data, 12 DMRS)  | PDSCH data + DMRS (rnti 0,1) | NRB 3 - END |
    SYM 12 | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    SYM 13 | PDSCH | NRB 0 - 2 | TOTAL REs = 36 (36 data)           | PDSCH data (rnti = 0, 1)     | NRB 3 - END |
    */

    /* 
    1) NR-TM-FR1-3.2 fixes the PDCCH to 1 CCE = 6 REGs spread over symbols 0–1 in RB 0-2. 
    A REG is one resource-block (12 sub-carriers) in one OFDM symbol, so
                6REGs x 12 Sub-carriers = 72 REs over symbol 0 and 1
    36 RE in symbol 0 (3 RB × 12)
    36 RE in symbol 1 (3 RB × 12) 
    REs locations for PDCCH-DMRS is calculated in clause 7.4.1.3.2 in 38.211 as:
        k = nN^RB_SC + 4k' + 1 
        where,
            N^RB_SC is the number of subcarriers per RB
            k' = 0, 1, 2 is the frequency offset per RB
            n = 0,1 .. N^CORESET_RB - 1; N^CORESET_RB is the number of RBs for PDCCH 
    since N^CORESET_RB = 3, n = 0, 1, 2 and hence there are 9 possible 'k' values.
    This implies number of REs for PDCCH = 9 in a symbol 
    
    2) PDSCH-DMRS : For DMRS-TypeA, only 1 symbol is mandatory but for this test model additionalDMRSdsymbol = 1
    hence Symbol 2 and Symbol 11 are for DMRS. Also DMRS is 6 REs/REG. For this test model Non-DMRS REs are filled with
    PDSCH data. 

    3) RNTI = 1 is used for EVM calculation
    */

static void build_pdcch_subcarrierindices(pdcch_params_t* m, prb_vec_t *prbinfo) {

    // total counts
    m->pdcchdata.n = 9 * prbinfo->n;
    m->pdcchdmrs.n = 3 * prbinfo->n;

    m->pdcchdata.idx  = mem_alloc(sizeof(uint16_t) * m->pdcchdata.n);  assert(m->pdcchdata.idx);
    m->pdcchdmrs.idx  = mem_alloc(sizeof(uint16_t) * m->pdcchdmrs.n);  assert(m->pdcchdmrs.idx);

    uint16_t *w_data = m->pdcchdata.idx;
    uint16_t *w_dmrs = m->pdcchdmrs.idx;

    for (uint16_t k = 0; k < prbinfo->n; k++) {

        const uint16_t base = (uint16_t)(12 * prbinfo->idx[k]);

        // write 9 data indices for this PRB
        for (int i = 0; i < 9; i++) {
            w_data[i] = (uint16_t)(base + PDCCH_DATA_OFS[i]);
        }
        w_data += 9;

        // write 3 dmrs indices for this PRB
        for (int j = 0; j < 3; j++) {
            w_dmrs[j] = (uint16_t)(base + PDCCH_DMRS_OFS[j]);
        }
        w_dmrs += 3;
    }
}

/**
 * @brief   This function is used to calculate the subcarrier indices for PDSCH data and dmrs for symbols where
 *          dmrs is present. For non-dmrs symbols, this function is not called
 */
static void build_pdsch_subcarrierindices(re_vec_t* pdschdmrs, re_vec_t* pdschdata, prb_vec_t* prbinfo, dmrs_comb_t comb) {

    assert(comb == DMRS_COMB_2); // only comb_2 is implemented for now

    pdschdmrs->n = pdschdata->n = ( (prbinfo->n * NSCRB) >> 1 ); // half SCs belong to either in COMB_2
    pdschdmrs->idx = mem_alloc(sizeof(uint16_t) * pdschdmrs->n); assert(pdschdmrs->idx);
    pdschdata->idx = mem_alloc(sizeof(uint16_t) * pdschdata->n); assert(pdschdata->idx);
    
    uint16_t *w_dmrs = pdschdmrs->idx;
    uint16_t *w_data = pdschdata->idx;

    for(uint16_t prb = 0; prb < prbinfo->n; prb++) {
        const uint16_t base = (uint16_t)(12 * prbinfo->idx[prb]);

        // write DMRS indicies
        for(uint16_t i = 0; i < 6; i++) {
            w_dmrs[i] = (uint16_t)(base + PDSCH_DMRS_OFS[i]);
        }
        w_dmrs += 6;

        for(uint16_t i = 0; i < 6; i++) {
            w_data[i] = (uint16_t)(base + PDSCH_DATA_OFS[i]);
        }
        w_data += 6;
    }
}

void rem_metadata_gen(rem_metadata_t* rem_metadata, uint32_t nrb, nr_scs_khz_t scs, int bw) {

    assert(rem_metadata);
    rem_metadata->plan.pdcch_symmask = 0x3; //0000 0000 0000 0011 sym 0 and 1
    rem_metadata->plan.dmrs_symmask_ue[0] = 0x804; //0000 1000 0000 0100 sym 2 and 11
    rem_metadata->plan.pdsch_symmask_ue[0] = 0x3FFF; // 0011 1111 1111 1111 sym 0 to 13

    rem_metadata->plan.dmrs_symmask_ue[1] = 0x804; //0000 1000 0000 0100 sym 2 and 11
    rem_metadata->plan.pdsch_symmask_ue[1] = 0x3FFF; // 0011 1111 1111 1111 sym 0 to 13

    rem_metadata->plan.dmrs_symmask_ue[2] = 0x204; //0000 0010 0000 0100 sym 2 and 9
    rem_metadata->plan.pdsch_symmask_ue[2] = 0x3FFC; // 0011 1111 1111 1100 sym 2 to 13

    rem_metadata->dmrs.comb = DMRS_COMB_2;
    rem_metadata->dmrs.add_pos = 1; // in addition to sym 2, DMRS will also be in sym11

    NR_FR1_TM32_t* nrfr1tm32 = mem_alloc(sizeof(NR_FR1_TM32_t));
    assert(nrfr1tm32);

    NR_FR1_TM3_2(nrb, nrfr1tm32);

    /* PDCCH */
    build_pdcch_subcarrierindices(&rem_metadata->pdcchparams, &nrfr1tm32->pdcchprb);

    /* PDSCH */
    for (uint8_t ue = 0; ue < 3; ue++) { 
        for (uint8_t s = 0; s < 14; s++) {

            // For symbols with PDSCH-DMRS
            if ( ( (rem_metadata->plan.pdsch_symmask_ue[ue] >> s) & 0x1 ) &&
                 ( (rem_metadata->plan.dmrs_symmask_ue[ue]  >> s) & 0x1 ) ) {
                    build_pdsch_subcarrierindices( &rem_metadata->pdschparams.pdschdmrsue[ue][s], 
                                                   &rem_metadata->pdschparams.pdschdataue[ue][s], 
                                                   &nrfr1tm32->pdschprb[ue],
                                                   rem_metadata->dmrs.comb);
                }
            
            // For symbols with PDSCH-DATA only
            if ( ( (rem_metadata->plan.pdsch_symmask_ue[ue] >> s) & 0x1 ) &&
                 !( (rem_metadata->plan.dmrs_symmask_ue[ue] >> s) & 0x1 ) ) {
                    prb_vec_t *src = &nrfr1tm32->pdschprb[ue];
                    prb_vec_t *dest = &rem_metadata->pdschparams.pdsch_prbs_ue[ue][s];
                    dest->n = src->n;
                    dest->idx = mem_alloc(sizeof(uint16_t) * dest->n); assert(dest->idx);
                    memcpy(dest->idx, src->idx, sizeof(uint16_t) * dest->n);
                }
            
        } // all OFDM symbols
    } // 3 RNTIs

    nr_evm_entry_t* tbl = nr_evm_lookup(scs, bw);
    assert(tbl);
    rem_metadata->nfft = tbl->nfft;

    mem_free(nrfr1tm32);

}

/**
 * @brief   Main.c will decide whether buf is pointing to ping or pong
 */
void resource_element_mapping(rem_metadata_t* rem_metadata, uint8_t symidx, complex_t *buf, channelsym_t* channelsym) {

    assert(buf); assert(channelsym); assert(rem_metadata);

    /* PDCCH */
    if ( (rem_metadata->plan.pdcch_symmask >> symidx) & 0x1 ) {
        // PDCCH is present in this symbol
        const re_vec_t *dt = &rem_metadata->pdcchparams.pdcchdata;
        const re_vec_t *dm = &rem_metadata->pdcchparams.pdcchdmrs;

        for(uint16_t i = 0; i < dt->n; i++) {
            //iterate over all the data subcarrier indices for pdcch
            assert(pdcchdataREidx < channelsym->pdcch.data.n);
            const uint16_t dst = dt->idx[i];
            buf[dst] = channelsym->pdcch.data.s[pdcchdataREidx++];
        }

        for(uint16_t j = 0; j < dm->n; j++) {
            //iterate over all the dmrs subcarrier indices for pdcch
            assert(pdcchdmrsREidx < channelsym->pdcch.control.n);
            const uint16_t dst = dm->idx[j];
            buf[dst] = channelsym->pdcch.control.s[pdcchdmrsREidx++];
        }
    }

    /* PDSCH */
    for (uint8_t ue = 0; ue < 3; ue++) {
        // iterate over all UEs
        if ( (rem_metadata->plan.dmrs_symmask_ue[ue] >> symidx) & 0x1) {
            // dmrs for ue is present in this symbol
            
            const re_vec_t *dm = &rem_metadata->pdschparams.pdschdmrsue[ue][symidx];
            const re_vec_t *dt = &rem_metadata->pdschparams.pdschdataue[ue][symidx];

            // fill pDSCH DMRS indices first
            for (uint16_t re = 0; re < dm->n; re++) {
                // iterate over REs
                assert(pdschdmrsREidx[ue] < channelsym->pdsch[ue].control.n);
                const uint16_t dst = dm->idx[re];
                buf[dst] = channelsym->pdsch[ue].control.s[pdschdmrsREidx[ue]++];
            }
            // fill PDSCH data indices now
            for (uint16_t re = 0; re < rem_metadata->pdschparams.pdschdataue[ue][symidx].n; re++) {
                // iterate over REs
                assert(pdschdataREidx[ue] < channelsym->pdsch[ue].data.n);
                const uint16_t dst = dt->idx[re];
                buf[dst] = channelsym->pdsch[ue].data.s[pdschdataREidx[ue]++];
            }
        }
        else if ( ( rem_metadata->plan.pdsch_symmask_ue[ue] >> symidx) & 0x1) {
            // PDSCH-data only symbol
            const prb_vec_t *pv = &rem_metadata->pdschparams.pdsch_prbs_ue[ue][symidx];

            for (uint16_t prb = 0; prb < pv->n; prb++) {
                // iterate over all allocated PRBs
                const size_t base = (size_t)12 * pv->idx[prb];
                complex_t *dst = &buf[base];
                const complex_t *src = &channelsym->pdsch[ue].data.s[pdschdataREidx[ue]];
                memcpy(dst, src, sizeof(complex_t) * 12);
                pdschdataREidx[ue] += 12;
            }
        }
    }

}

/**
 * @brief   Call them before each slot begins
 */
void rem_reset_counters() {
    pdcchdataREidx = pdcchdmrsREidx = 0;
    for (int u = 0; u < 3; ++u) { pdschdmrsREidx[u] = pdschdataREidx[u] = 0; }
}