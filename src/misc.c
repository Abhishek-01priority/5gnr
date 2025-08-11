#include "misc.h"

uint8_t * read_bits_from_file(const char *fname, size_t *nBits) {
    // open file
    FILE *fp = fopen(fname, "rb");
    assert(fp);

    // find file size
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // read all bytes
    uint8_t * bytes = mem_alloc(fsize);
    if (fread(bytes, 1, fsize, fp) != (size_t)fsize) {
        perror("fread");
        assert(0);
    }
    fclose(fp);

    // store the data in array
    *nBits = fsize * 8;
    uint8_t * bits = malloc(*nBits);
    for (long i = 0; i < fsize; i++) {
        uint8_t b = bytes[i];
        for (int bit = 0; bit < 8; bit++) {
            bits[i*8 + bit] = (b >> (7 - bit) ) & 0x1;
        }
    }

    free(bytes);

    return bits;
}

void write_symbols_to_file(char *fname, complex_t *sym, uint32_t numsym) {
    
    FILE *fp = NULL; fp = fopen(fname, "wb");
    assert(fp);
    
    for (uint32_t s = 0; s < numsym; s++) {
        fwrite(&sym[s].i, sizeof(int16_t), 1, fp);
        fwrite(&sym[s].q, sizeof(int16_t), 1, fp);
    }
    fclose(fp);
}

/* Matlab/octave code for reading 
fid = fopen('constellation.bin','rb');
d = fread(fid, 'int16', 0, 'ieee-le'); fclose(fid);

assert(mod(numel(d),2)==0, 'Expected interleaved I,Q int16s.');
I = double(d(1:2:end))/256;   % Q8.8 -> float
Q = double(d(2:2:end))/256;
sym = I + 1j*Q;

plot(real(sym), imag(sym), 'o'); grid on; axis equal;
xlabel('I'); ylabel('Q'); title('Constellation');
*/