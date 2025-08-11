#ifndef TESTCASEPARAMSGEN_H
#define TESTCASEPARAMSGEN_H


#include "common.h"

typedef struct {
    int level_of_deboosting;
    int level_of_boosting;
    const uint8_t Nstartbwp;
    uint64_t pdsch_mod_and_boost_bitmap; // right to left, 1 -> deboosted 16 QAM, 0 -> boosted QPSK
} NR_FR1_TM32_t;

int fetch_nrbs(int scs/*in KHz*/, int bw /*in MHz*/);
int NR_FR1_TM3_2(int nrb, NR_FR1_TM32_t* nrfr1tm32);

#endif