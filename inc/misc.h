#ifndef MISC_H
#define MISC_H

#include "common.h"
#include "mem_alloc.h"
#include "modulation.h"
#include "bit_and_symbol.h"

uint8_t * read_bits_from_file(const char *fname, size_t *nBits);
void write_symbols_to_file(char *fname, complex_t *sym, uint32_t numsym);
void read_symbols_from_file(const char* fname, syminfo_t *syminfo);

#endif