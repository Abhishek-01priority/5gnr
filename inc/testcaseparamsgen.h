#ifndef TESTCASEPARAMSGEN_H
#define TESTCASEPARAMSGEN_H


#include "common.h"
#include "resource_element_mapping.h"

typedef struct {
    int level_of_deboosting;
    int level_of_boosting;
    const uint8_t Nstartbwp;
    prb_vec_t pdschprb[3]; // for each UE
    prb_vec_t pdcchprb;

} NR_FR1_TM32_t;

uint32_t fetch_nrbs(int scs/*in KHz*/, int bw /*in MHz*/);
int NR_FR1_TM3_2(uint32_t nrb, NR_FR1_TM32_t* nrfr1tm32);

/**
 * @brief DL evm test models calculate the allocations in terms of RBGs. For resource element mapping purpose these RBGs
 *        need to be converted to PRBs.
 * @param rbg       pointer type, should be freed after this function call
 * @param nrbg      number of RBGs
 * @param nrb       For keeping a check on prbarray
 * @param P         PRB to RBG ratio, taken from Table 5.1.2.1-1 38.214 v17.1.0
 * @param out_n     pointer type, number of PRBs filled
 * @return prb_indices Number of PRB indices filled
 */
uint16_t * rbgarray_to_prbarray(const uint16_t *rbg, size_t nrbg,
                         uint16_t nrb, uint8_t P,
                         size_t *out_n, size_t *prbbitmap);

#endif