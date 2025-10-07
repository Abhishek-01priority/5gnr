#ifndef COMMON_H
#define COMMON_H

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


#endif