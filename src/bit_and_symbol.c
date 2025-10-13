#include "misc.h"
#include "common.h"
#include "mem_alloc.h"
#include "bit_and_symbol.h"

// #define __DEBUG__


/**
 * @brief   Helper function-> wrapper over modulation so that code is not repeated
 * @param   [in] bits_t *bits
 * @param   [in] modulationtype_t modulationtype
 * @return  complex_t *symbol
 */
static void perform_modulation(bitinfo_t* ch, modulationtype_t modulationtype, syminfo_t* syminfo) {

    assert(syminfo); assert(ch);

    if (modulationtype == QPSK) {
        syminfo->n = (uint16_t) (ch->numbits >> 2);
    }
    else if(modulationtype == QAM16) {
        syminfo->n = (uint16_t) (ch->numbits >> 4);
    }
    syminfo->s = mem_alloc(sizeof(uint16_t) * syminfo->n); assert(syminfo->s);

    uint8_t dibit, nibble;
    if (modulationtype == QPSK) {
        for (uint16_t s = 0; s < syminfo->n; s++) {
            dibit = ( (ch->bits[s*2 + 0] << 1) | (ch->bits[s*2 + 1] << 0) );
            complex_t *sym = &syminfo->s[s];
            qpskmod(dibit, sym);
        }
    }
    else if (modulationtype == QAM16) {
        for (uint16_t s = 0; s < syminfo->n; s++) {
            nibble = ((ch->bits[s * 4 + 0] << 3) | (ch->bits[s * 4 + 1] << 2) | (ch->bits[s * 4 + 2] << 1) | (ch->bits[s * 4 + 3] << 0));
            complex_t *sym = &syminfo->s[s];
            qammod_16(nibble, sym);
        }
    }
#ifdef __DEBUG__
    write_symbols_to_file("debug\\constellation.bin", syminfo->s, syminfo->n);
#endif

}

void bitgen(bits_t * bits) {

    assert(bits);
    char *fname;

    fname = "data\\pdsch_rnti0_databits.bin";
    bits->pdschrnti0.bits = read_bits_from_file(fname, &bits->pdschrnti0.numbits);

    fname = "data\\pdsch_rnti1_databits.bin";
    bits->pdschrnti1.bits = read_bits_from_file(fname, &bits->pdschrnti1.numbits);

    fname = "data\\pdsch_rnti2_databits.bin";
    bits->pdschrnti2.bits = read_bits_from_file(fname, &bits->pdschrnti2.numbits);

    fname = "data\\pdcch_databits.bin";
    bits->pdcch.bits = read_bits_from_file(fname, &bits->pdcch.numbits);
}

void symbolgen(bits_t* bits, channelsym_t *channelsym) {

    // Generate symbols for all data channel
    // PDCCH Data
    perform_modulation(&bits->pdcch, QPSK, &channelsym->pdcch.data);

    // PDSCH RNTI-0 Data
    perform_modulation(&bits->pdschrnti0, QPSK, &channelsym->pdsch[0].data);

    // PDSCH RNTI-1 Data
    perform_modulation(&bits->pdschrnti1, QAM16, &channelsym->pdsch[1].data);

    // PDSCH RNTI-2 Data
    perform_modulation(&bits->pdschrnti2, QPSK, &channelsym->pdsch[2].data);

    char *fname;
    // PDCCH DMRS
    fname = "data\\pdcch_dmrssyms.bin";
    read_symbols_from_file(fname, &channelsym->pdcch.control);

    // PDSCH RNTI-0 DMRS
    fname = "data\\pdsch_rnti0_dmrssyms.bin";
    read_symbols_from_file(fname, &channelsym->pdsch[0].control);

    // PDSCH RNTI-1 DMRS
    fname = "data\\pdsch_rnti1_dmrssyms.bin";
    read_symbols_from_file(fname, &channelsym->pdsch[1].control);

    // PDSCH RNTI-2 DMRS
    fname = "data\\pdsch_rnti2_dmrssyms.bin";
    read_symbols_from_file(fname, &channelsym->pdsch[2].control);

}