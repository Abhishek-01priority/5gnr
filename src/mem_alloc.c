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

void mem_free(void *ptr) {

    assert(ptr);
    #ifdef __MALLOC__
        free(ptr);
    #else
        #error "Not implemented yet"
    #endif
    ptr = NULL;
}