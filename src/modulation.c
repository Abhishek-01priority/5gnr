#include "modulation.h"

/* Declaring these constants as static: 
Using static on a file‑scope constant does two things:

Internal linkage
By default in C, a const variable at file scope has external linkage (it’s visible to the linker and other translation units). 
Marking it static confines it to the current .c file, avoiding name collisions if another module also defines a norm or one_by_sqrt10.

Single allocation
Even if you declared it inside a function, static gives it static storage duration—it lives for the entire run of the program (not on the stack each call).
For a compile‑time constant the compiler will often optimize it away, but static makes your intent explicit.*/

static const int16_t norm               = (uint8_t)(0.70710678 * (1 << 8) + 0.5);
static const int16_t one_by_sqrt10      = (uint8_t)(0.31622777 * (1 << 8) + 0.5);
static const int16_t three_by_sqrt10    = (uint8_t)(0.94868330 * (1 << 8) + 0.5);

void qpskmod(uint8_t bits /*2 bits*/, complex_t* symbol) {
    assert(bits < 4);

    switch (bits) {
        case 0 : symbol->i = +norm; symbol->q = +norm; break;
        case 1 : symbol->i = -norm; symbol->q = +norm; break;
        case 2 : symbol->i = +norm; symbol->q = -norm; break;
        case 3 : symbol->i = -norm; symbol->q = -norm; break;
        default: assert(0);
    }
}

void qammod_16(uint8_t bits, complex_t *sym) {
    assert(bits < 16);

    switch(bits) {
        // --- I = –3 ---
        case 0x0: /*0000*/ sym->i = -three_by_sqrt10; sym->q = -three_by_sqrt10; break;  
        case 0x1: /*0001*/ sym->i = -three_by_sqrt10; sym->q = -one_by_sqrt10;   break;  
        case 0x3: /*0011*/ sym->i = -three_by_sqrt10; sym->q = +one_by_sqrt10;   break;  
        case 0x2: /*0010*/ sym->i = -three_by_sqrt10; sym->q = +three_by_sqrt10; break;  

        // --- I = –1 ---
        case 0x4: /*0100*/ sym->i = -one_by_sqrt10;   sym->q = -three_by_sqrt10; break;  
        case 0x5: /*0101*/ sym->i = -one_by_sqrt10;   sym->q = -one_by_sqrt10;   break;  
        case 0x7: /*0111*/ sym->i = -one_by_sqrt10;   sym->q = +one_by_sqrt10;   break;  
        case 0x6: /*0110*/ sym->i = -one_by_sqrt10;   sym->q = +three_by_sqrt10; break;  

        // --- I = +1 ---
        case 0xC: /*1100*/ sym->i = +one_by_sqrt10;   sym->q = -three_by_sqrt10; break;  
        case 0xD: /*1101*/ sym->i = +one_by_sqrt10;   sym->q = -one_by_sqrt10;   break;  
        case 0xF: /*1111*/ sym->i = +one_by_sqrt10;   sym->q = +one_by_sqrt10;   break;  
        case 0xE: /*1110*/ sym->i = +one_by_sqrt10;   sym->q = +three_by_sqrt10; break;  

        // --- I = +3 ---
        case 0x8: /*1000*/ sym->i = +three_by_sqrt10; sym->q = -three_by_sqrt10; break;  
        case 0x9: /*1001*/ sym->i = +three_by_sqrt10; sym->q = -one_by_sqrt10;   break;  
        case 0xB: /*1011*/ sym->i = +three_by_sqrt10; sym->q = +one_by_sqrt10;   break;  
        case 0xA: /*1010*/ sym->i = +three_by_sqrt10; sym->q = +three_by_sqrt10; break;  

        default:
            assert(0);
    }
}
