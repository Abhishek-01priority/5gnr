#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include "common.h"

#define __MALLOC__

void* mem_alloc(size_t size);
void mem_free(void *ptr);

#endif