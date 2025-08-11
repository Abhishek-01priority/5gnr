#include "mem_alloc.h"

void* mem_alloc(size_t size) {

    void* ptr = NULL;
    #ifdef __MALLOC__
        ptr = malloc(size);
    #else
        #error "MEMPOOLs not implemented yet"
    #endif

    return ptr;

}