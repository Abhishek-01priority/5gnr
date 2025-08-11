#include "common.h"
#include "mem_alloc.h"

#define TAKE_MATLAB_GENERATED_BITS

#ifndef TAKE_MATLAB_GENERATED_BITS
    #error "not implemented"
#endif


void resource_element_mapping() {

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
}