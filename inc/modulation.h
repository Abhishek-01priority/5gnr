#ifndef MODULATION_H
#define MODULATION_H

#include "common.h"

typedef struct {
    int16_t i; // Q8.8 format
    int16_t q; // Q8.8 format
} complex_t;

void qpskmod(uint8_t bits /*2 bits*/, complex_t* symbol);

/*
\brief Perform 16 QAM Gray-mapped modulation using lookup table
*/
void qammod_16(uint8_t bits, complex_t *sym);

#endif