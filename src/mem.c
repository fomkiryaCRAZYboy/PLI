#include "mem.h"
#include <stdlib.h>
#include <stdio.h>

/* start of memory chain, it leads to NULL */
static MEMORY_BLOCK* memory_chain = NULL;

void* pli_alloc(size_t size) 
{
    MEMORY_BLOCK* block = malloc (sizeof(MEMORY_BLOCK));
    if (!block)
        return NULL ; 
    

    void* ptr = malloc (size);
    if (!ptr)
    {
        free (block);
        return NULL ;
    }

    block -> ptr  = ptr;
    block -> prev = memory_chain;
    memory_chain  = block;

    return ptr ;
}

FREE_RES pli_free(void* ptr)
{
    if (!ptr) 
        return SUCCESS_FREE ;

    MEMORY_BLOCK** current = &memory_chain;  
    
    while (*current) 
    {
        if ((*current)->ptr == ptr) 
        {
            MEMORY_BLOCK* to_free = *current;
            free(to_free->ptr);
            *current = to_free->prev;
            free(to_free);

            return SUCCESS_FREE ;
        }

        current = &(*current)->prev;
    }
    
    return FAILED_FREE ; /* trying to free untracked pointer */
}

/* clearing all memory */
void emergency_cleanup() 
{
    while (memory_chain)
    {
        if(memory_chain -> ptr)
            free (memory_chain -> ptr);

        MEMORY_BLOCK* tmp = memory_chain;
        memory_chain = memory_chain -> prev;
        free (tmp);
    }
}