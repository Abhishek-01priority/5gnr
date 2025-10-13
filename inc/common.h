#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

typedef enum {
    NR_TM_FR1_TM32
} testmodel_t;

typedef enum { 
    NR_SCS_15 = 15, 
    NR_SCS_30 = 30, 
    NR_SCS_60 = 60 
} nr_scs_khz_t;

typedef enum {
    BW_5_MHZ = 0,
    BW_10_MHZ,
    BW_15_MHZ,
    BW_20_MHZ,
    BW_25_MHZ,
    BW_30_MHZ,
    BW_35_MHZ,
    BW_40_MHZ,
    BW_45_MHZ,
    BW_50_MHZ,
    BW_60_MHZ,
    BW_70_MHZ,
    BW_80_MHZ,
    BW_90_MHZ,
    BW_100_MHZ,
    BW_NUM  // total number of bandwidths
} nr_bandwidth_t;

typedef struct {
    int16_t i; // Q8.8 format
    int16_t q; // Q8.8 format
} complex_t;

typedef struct {             
    uint16_t *idx; // PRB indices
    size_t   n;   // number of PRBs

} prb_vec_t; // variable-length PRB list for a symbol

#endif