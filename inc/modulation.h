#ifndef MODULATION_H
#define MODULATION_H

#include "common.h"

void qpskmod(uint8_t bits /*2 bits*/, complex_t* symbol);

/*
\brief Perform 16 QAM Gray-mapped modulation using lookup table
*/
void qammod_16(uint8_t bits, complex_t *sym);

#endif