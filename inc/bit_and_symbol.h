#ifndef BIT_AND_SYMBOL_H
#define BIT_AND_SYMBOL_H

#include "common.h"
#include "modulation.h"

typedef struct {
    size_t numbits;
    uint8_t *bits;
} bitinfo_t;

typedef struct {
    bitinfo_t pdcch;
    bitinfo_t pdschrnti0;
    bitinfo_t pdschrnti1;
    bitinfo_t pdschrnti2;
    /* DMRS bits for PDCCH and PDSCH are not generated, DMRS symbols are directly taken from matlab */
} bits_t;

typedef enum {
    QPSK = 0, QAM16 = 1
} modulationtype_t;

typedef struct {
    complex_t* s; // complex symbols
    uint16_t   n; // number of symbols
} syminfo_t;

typedef struct {
    syminfo_t data;
    syminfo_t control;
} channeltype_t;

typedef struct {
    channeltype_t pdcch;
    channeltype_t pdsch[3];
} channelsym_t;

/* function declarations */
/**
 * @brief   Generate bits for PDCCH, PDSCH (rnti 0, 1, 2) and DMRS
 * @return  bits_t
 */
void bitgen(bits_t * bits);

/**
 * @brief   Generate complex symbols from bits, right now DMRS symbols are taken from Matlab
 */
void symbolgen(bits_t* bits, channelsym_t *channelsym);

#endif