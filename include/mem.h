#ifndef BASE_H
#define BASE_H

#include <stddef.h>

typedef enum
{
    SUCCESS_FREE ,
    FAILED_FREE  
}
FREE_RES ;

/* chain that linking pointers to allocated memory */
typedef struct memory_block
{
    struct memory_block* prev ;
    void* ptr ;
} 
MEMORY_BLOCK ;

void* pli_alloc(size_t size) ;
void emergeny_cleanup() ;
FREE_RES pli_free(void* ptr) ;

#endif /* BASE_H */