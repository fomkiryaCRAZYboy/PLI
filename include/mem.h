#ifndef BASE_H
#define BASE_H

#include <stddef.h>

/* chain that linking pointers to allocated memory */
typedef struct memory_block
{
    struct memory_block* prev ;
    void* ptr ;
} 
MEMORY_BLOCK ;

void* pli_alloc(size_t size) ;
void emergency_cleanup() ;
void pli_free(void* ptr) ;
void* pli_realloc(void* old_ptr, size_t new_size);

#endif /* BASE_H */