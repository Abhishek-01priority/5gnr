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
    uint8_t * bits = mem_alloc(*nBits);
    for (long i = 0; i < fsize; i++) {
        uint8_t b = bytes[i];
        for (int bit = 0; bit < 8; bit++) {
            bits[i*8 + bit] = (b >> (7 - bit) ) & 0x1;
        }
    }

    mem_free(bytes);

    return bits;
}

void read_symbols_from_file(const char* fname, syminfo_t *syminfo) {

    FILE *f = NULL; f = fopen(fname, "rb"); assert(f);

    // Determine file size
    fseek(f, 0, SEEK_END);
    long nbytes = ftell(f); assert(nbytes > 0);
    fseek(f, 0, SEEK_SET);

    size_t nsamp = (size_t)(nbytes / (2 * sizeof(int16_t))); // complex samples
    int16_t *buf = (int16_t *)mem_alloc(2 * nsamp * sizeof(int16_t));
    assert(buf);

    size_t nread = fread(buf, sizeof(int16_t), 2 * nsamp, f);
    fclose(f);
    assert(nread != 2*nsamp);

    syminfo = mem_alloc(sizeof(syminfo_t)); assert(syminfo);
    syminfo->n = nsamp; // number of complex symbols
    syminfo->s = mem_alloc(sizeof(complex_t) * syminfo->n); assert(syminfo->s);

    for (uint16_t k = 0; k < nsamp; k++) {
        // data already stored in Q8.8 format
        syminfo->s[k].i = buf[2 * k + 0];
        syminfo->s[k].q = buf[2 * k + 1];
    }

    mem_free(buf);

}

void write_symbols_to_file(char *fname, complex_t *sym, uint32_t numsym) {
    
    FILE *fp = NULL; fp = fopen(fname, "wb");
    assert(fp);
    
    for (uint32_t s = 0; s < numsym; s++) {
        fwrite(&sym[s].i, sizeof(int16_t), 1, fp);
        fwrite(&sym[s].q, sizeof(int16_t), 1, fp);
    }
    fclose(fp);

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
}

/* Matlab code to write the complex symbols in Q8.8 format to a file
x = dlresourceinfo.WaveformResources.PDCCH.Resources.DMRSSymbols; % complex Nx1

scale = 2^8;                                % Q8.8
xi = int16(max(min(round(real(x)*scale),  32767), -32768));
xq = int16(max(min(round(imag(x)*scale),  32767), -32768));

iq = zeros(2*numel(x),1,'int16');           % interleave I,Q
iq(1:2:end) = xi;
iq(2:2:end) = xq;

fid = fopen('D:\5gnr\data\pdcch_dmrssyms.bin','wb'); % binary, little-endian
fwrite(fid, iq, 'int16');
fclose(fid);


*/